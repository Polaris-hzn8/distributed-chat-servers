# 负载均衡与redis发布订阅

---

如果很多客户端连接1台服务器，进行业务操作没有什么问题，

但是1台服务器在32位linux操作系统，大致的并发量（socketfd1024，通过limit将进程最多能使用的文件描述符调大）支持2万左右。

即单台服务器最多支持2万左右的用户在线，

### 1.如何扩展并发量？

<mark>横向扩展chatServer的数量</mark>：从而提高并发服务器量，

- 如果想提高服务器的并发能力，让更多的用户同时上线，就需要进行集群（在水平方向上扩展多台主机，每一台主机运行的都是独立的chatServer）或者分布式部署，
- 这时客户端向服务端发起连接并没有指定连接哪一台业务服务器，如果都连接同一台服务器将导致连接量过大，服务器压力过高，需要通过nginx（轻松支持5~6万并发）来作为中转指定连接服务端的某台服务器（负载均衡）
- 如果想要继续提升并发量，可以对负载均衡器本身再次集群，负载均衡之前可以使用LVS（偏底层的负载均衡器扩展到十多万并发），负载均衡包含业务负载均衡、传输层负载（通过TCP\UDP分发）、网络层分发（通过IP分发）、数据链路层负载（通过数据帧来分发）

![image-20230510090338969](https://s2.loli.net/2023/05/10/JuH1pkNIFW4gtmY.png)

将3台chatServer服务器，配合一台nginx负载均衡服务器，将并发量扩展至5~6万，

<mark>负载均衡器的作用</mark>：

1. 用户在连接客户端时就不用去连接每一台chatServer了，可以直接连接负载均衡服务器，负载均衡器将client的请求按照负载算法（轮询、按权重、按IP哈希）分发到具体的业务服务器上chatServer，
2. 负载均衡器还需要和ChatServer保持心跳机制，监测chatServer是否故障。（每一次发送一个心跳，就将心跳接收器-1，发送了心跳之后没有得到响应，则将心跳接收器+1，如果心跳接收器+3多次心跳未响应，说明网路不通判断为服务器不能正常提供服务）
3. 能够发现新添加的chatServer设备，方便扩展服务器的数量，在不影响原有正常业务服务器的情况下，动态的添加业务服务器数量，而不用将整个服务端重启。

聊天服务器属于长连接业务，客户端与服务器建立连接后需要一直保持，因为客户端与服务器之间需要一直进行通信与信息传递。

<mark>网络服务器编程常用模型</mark>：

1. 方案1：accept + read/write（非并发服务器）
2. 方案2：accept + fork（process pre connection）
3. 方案3：accept + thread（thread pre connection）
4. 方案4：reactors in threads （one loop per thread）：有一个main reactor负载accept连接，然后将连接分发到某个sub reactor（采用round-robin的方式来选择sub reactor），该连接的所有操作都在sub reactor所处的线程中完成，多个连接可能被分派到多个线程中，以充分利用CPU。
5. reactors in process（one loop per process）：nginx服务器的网络模块设计，基于进程设计采用多个Reacors充当IO进程和工作进程，通过accept锁解决多个Reactors的惊群现象。

### 2.集群之后出现的通信问题？

<mark>如何解决跨业务服务器通信的问题</mark> ? 有些公有信息需要在集群的多台服务器之间进行共享，例如当前在线的用户，

#### part1

- 集群部署的服务器之间进行通信，最好的方式就是<font color='#BAOC2F'>引入中间件消息队列</font>，解耦各个服务器，使整个系统松耦合。
- 提高服务器的响应能力，节省服务器的带宽资源，每添加一天服务器，只需根据消息队列建立连接即可，不需与其他服务器直接建立连接
- 在集群分布式环境中，常使用的中间件消息队列有ActiveMQ、RabbitMQ、Kafka、都是应用场景广泛、且性能好的消息队列，
- 为集群服务器之间、分布式服务之间提供消息通信，
- 本项目使用的中间件消息队列是基于发布订阅模式的redis，

![image-20230510111634920](https://s2.loli.net/2023/05/10/WrEx4SVlM1JPH83.png)

redis在该项目当中是作为服务器中间件消息队列使用的，来解耦服务器之间硬链接的设计方案，

1. 当用户在某台业务服务器登录成功后，集群中的每一台服务器都会会消息队列进行连接，向消息队列中写入数据，以及从消息队列中取出数据。
2. 每一次有用户在chatServer1中登录后，chatServer1都需要订阅一下消息队列，如果用户杂chatServer2中登录，则chatServer2订阅消息队列（对client2的消息感兴趣），
3. 在消息队列中如果有人给client1发送消息，消息队列就向chatServer1上报数据，
4. 当处于不同服务器上的用户与好友之间通信时，用户先发布publish chat json到消息队列中，
5. 由于好友所在的服务器已经订阅的相关消息，消息队列会将chat json notify到好友所在服务器上
6. 从而，服务器所需要的工作就是向消息队列订阅消息subscribe、发布消息publish，消息队列向服务器主动通知notify（观察者模式）

#### part2

当某个用户在chatServer1上成功登录后，chatServer1需要在redis上订阅一个通道channel1（对该通道上发生的消息十分关注），

当在该通道上发生消息时需要redis向chatServer1上报，

当某个用户在chatServer2上成功登录后，chatServer2需要在redis上订阅一个通道channel2（对该通道上发生的消息十分关注），

当在该通道上发生消息时需要redis向chatServer2上报，

subscribe、publish

- 这里以用户的id号为redis的通道号channel，来订阅消息
- 当某个chatServer服务器上的某个用户想给好友发送消息，而好友与他并不在同一台服务器上时，需要向redis上订阅的通道publish发布消息
- 好友登录所在的服务器在redis上订阅了消息，redis会将publish消息发送给好友，这样用户就能够与不同服务上的好友进行交流了。

#### part3

redis提供的功能，

1. 客户端能够连接redis
2. 客户端能够向redis订阅通道channel
3. 客户端能向redis通道发布消息publish
4. 客户端能够向redis取消订阅的通道
5. redis接受到通道中的消息后，能够向客户端上报消息（观察者模式）
6. 需要实现注册一个回调，当通道上有消息发生的时候redis才能回调（通知客户端感兴趣的通道的消息发生了）

### 3.redis消息队列通信流程具体分析

- step1：在构造函数中连接redis服务器，连接成功后注册回调函数（便于redis检测到通道消息后进行消息上报）

```cpp
//将消息id 以及对应的Handler回调操作 初始化写入_msgHandlerMap中
ChatService::ChatService() {
    //unordered_map<int, MsgHandler> _msgHandlerMap;//存储消息id 及其对应的事件处理方法表
    //相应的消息id 及其 与对应的事件回调处理函数 做一个绑定
    _msgHandlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3) });//登录
    _msgHandlerMap.insert({REG_MSG, bind(&ChatService::regis, this, _1, _2, _3) });//注册
    _msgHandlerMap.insert({ACOUNT_FRESH_MSG, bind(&ChatService::sendNewestInfo, this, _1, _2, _3) });//更新客户端数据
    _msgHandlerMap.insert({LOGOUT_MSG, bind(&ChatService::clientClose, this, _1, _2, _3) });//更新客户端数据

    _msgHandlerMap.insert({ONE_CHAT_MSG, bind(&ChatService::chat, this, _1, _2, _3) });//单聊
    _msgHandlerMap.insert({ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3) });//添加好友

    _msgHandlerMap.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3) });//创建群聊
    _msgHandlerMap.insert({ADD_GROUP_MSG, bind(&ChatService::joinGroup, this, _1, _2, _3) });//加入群聊
    _msgHandlerMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3) });//发送群消息

    //连接redis服务器
    if (_redis.connect()) {
        /* 连接上redis服务器后需要预置一个回调 */
        /* 因为redis底层会监听通道上的消息 如果需要上报消息 则需要通过这个回调函数（预先注册回调函数） */
        /* 通过绑定器绑定服务中的方法 this对象及其预留参数 channel_id channel_id->str */
        
        //1.bind包装上报消息的方法
        function<void(int, string)> func = std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2);
        //2.redis设置上报通道消息的回调方法
        _redis.init_notify_handler(func);
    }
}
```

- step2：客户端login登录成功后，需要向redis服务器订阅通道（以uid订阅通道）

```cpp
//（3）用户以uid登录成功后 向redis订阅channel(uid)
_redis.subscribe(uid);//消息队列中注册uid
```

- step3：客户单logout登出后，需要向redis服务器取消订阅的通道（以uid取消订阅通道）

```cpp
//3.redis消息队列中取消订阅
_redis.unsubscribe(uid);
```

- step4：客户端异常断开连接后，需要向redis服务器取消订阅的通道（以uid取消订阅通道）


```cpp
//3.redis消息队列中取消订阅
_redis.unsubscribe(uid);
```

- step5：客户单发送单聊请求消息，好友与用户不在同一台服务器，需要redis消息队列转发到好友所在服务器上

```cpp
//4.目标用户如果在其他服务器上在线 则直接进行消息发送（将消息发送到redis消息队列上 由redis消息队列转发给对应用户）
User user = _userModel.query(to);
if (user.getState() == "online") {
    _redis.publish(to, js.dump());//将消息发布到redis的channel_id(to)通道上 == 事件上报
    return;
}
```

- step6：客户单发送群聊请求消息，群聊中的用户与发送用户不在同一台服务器，需要redis消息队列转发消息到群友所在服务器上

```cpp
//2.根据群用户不同在线状态 进行发送群消息 or 存储离线消息
vector<int> guids = _groupModel.queryGroupUsers(uid, gid);
/* c++中的map不是线程安全的map */
lock_guard<mutex> lock(_connMutex);
for (int guid : guids) {
    auto it  = _userConnMap.find(guid);
    if (it != _userConnMap.end()) {
        // case1 : 用户与好友处于同一台服务器上 并且好友处于在线状态 直接转发消息
        it->second->send(js.dump());
    } else {
        /* 目标用户在本台服务器上的_connMap中没有发现 */
        User user = _userModel.query(guid);
        if (user.getState() == "online") {
            // case2 : 用于处于不同服务器上 并且好友处于在线状态 通过redis消息队列向其他服务器发送消息
            _redis.publish(guid, js.dump());
        } else {
            // case3 : 用户的好友处于离线状态
            _offMessageModel.insert(uid, js.dump());
        }
    }
}
```

- step7：redis从发生消息的通道中上报消息到指定的服务器，做消息转发

```cpp
//redis调用的回调函数
void ChatService::handleRedisSubscribeMessage(int uid, string msg) {
    /* 消息会被redis转发到uid所在的服务器上 */
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(uid);
    if (it != _userConnMap.end()) {
        it->second->send(msg);
        return;
    }
    _offMessageModel.insert(uid, msg);
}
```



























