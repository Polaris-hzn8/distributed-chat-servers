# redis发布订阅问题

---

以redis的发布-订阅机制作为集群服务器的消息中间件，解耦服务器之间的通信。用hiredis做客户端编程。

1. bug 1. 向消息中间件PUBLISH发布消息不成功，导致无法跨服务器通信
2. bug 2. 多次向中间件SUBSCRIBE订阅消息，出现无响应，客户端也无响应

### bug1

两个客户端C1和C2分别登录在两台服务器S1和S2上，服务器通过redis作为消息中间件进行消息通信。当客户端C1和C2通信时，C1先发送消息到S1上，S1向redis上名字为C2的channel通道发布消息，S2服务器从C2的channel上接收消息后，再把消息转发给C2客户端，注意hiredis提供的发送redis命令行接口函数redisCommand，在发送发布-订阅命令时，需要在不同的上下文Context环境中进行，不能够在同一Context下进行。

```
// 下面redisCommand函数的第一个参数this->_publishContext需要使用一个单独的Context，不能
// 和subscribe使用一样的Context
redisReply *reply = (redisReply*)redisCommand(this->_publishContext, "PUBLISH %d %s", channel, message.c_str());
```



### bug2

客户端偶尔出现登录以后没有响应，造成业务不能继续进行，此时直接调试出现问题的服务器Server.out

查看出现问题的服务器的pid：

```
tony@tony-virtual-machine:~$ ps -ef | grep Server
tony       5904   1529  0 11:08 pts/1    00:00:00 ./Server.out 127.0.0.1 9000
tony       5909   1442  0 11:08 pts/0    00:00:00 ./Server.out 127.0.0.1 9002
tony       6004   5991  0 11:13 pts/4    00:00:00 grep --color=auto Server
```

从上面看到，出现问题的Server的PID是5909

#### gdb调试5909号进程：

![bug2-1](https://img-blog.csdnimg.cn/20190731113648716.png)

#### 打印当前进程所有的线程信息：

![bug2-2](https://img-blog.csdnimg.cn/20190731113810504.png)

用info threads可以输出当前进程所有线程的信息，可以看到Server.out是主线程，也就是muduo库的I/O线程，现在处理epoll_wait状态，等待新用户的连接；而EventLoop事件循环有三个线程，分别是ChatServer0、ChatServer1、ChatServer2，其中ChatServer1和ChatServer2处在epoll_wait状态，等待已连接用户的读写事件，但是ChatServer0却阻塞在__libc_recv函数处，不能继续处理逻辑业务，不能给客户端回复响应，导致客户端无应答。

#### 调试线程ChatServer0：

线程ChatServer0的thread id是2，如下：

![bug2-3](https://img-blog.csdnimg.cn/20190731114249215.png)

从上面的ChatServer0线程的调用堆栈打印信息（bt命令）可以看到，这个线程在处理客户端的登录请求时，需要向redis中间件消息队列订阅消息，是通过hiredis的redisCommand发送的subscribe订阅命令，但是通过调用堆栈信息查看，redisCommand不仅可以发送subscribe订阅命令到redis server，还会以阻塞的方式等待redis server的响应。

但实际上，项目代码上已经开启了一个独立的线程，专门做redisGetReply，如下：

![bug2-4](https://img-blog.csdnimg.cn/20190731114913317.png)

![bug2-5](https://img-blog.csdnimg.cn/20190731114924820.png)

由于线程池里面的redisGetReply抢了上面订阅subscribe的redisCommand底层调用的redisGetReply的响应消息，导致ChatServer0线程阻塞在这个接口调用上，无法再次回到epoll_wait处了，这个线程就废掉了，如果工作线程全部发生这种情况，最终服务器所有的工作线程就全部停止工作了！

### 解决方案

从hiredis的redisCommand源码上可以看出，它实际上相当于调用了这三个函数：

1. redisAppendCommand 把命令写入本地发送缓冲区
2. redisBufferWrite 把本地缓冲区的命令通过网络发送出去
3. redisGetReply 阻塞等待redis server响应消息

既然在muduo库的ThreadPool中单独开辟了一个线程池，接收this->_context上下文的响应消息，因此subcribe订阅消息只做消息发送，不做消息接收就可以了，如下：

```cpp
// 订阅通道
void subscribe(int channel) {
	// 只负责发送命令，不阻塞接收redis server响应消息，否则和notifyMsg线程抢占响应资源
	if (REDIS_ERR == redisAppendCommand(this->_context, "SUBSCRIBE %d", channel)) {
		LOG_ERROR << "subscribe [" << channel << "] error!";
		return;
	}
	// redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
	int done = 0;
	while (!done) {
		if (REDIS_ERR == redisBufferWrite(this->_context, &done)) {
			LOG_ERROR << "subscribe [" << channel << "] error!";
			return;
		}
	}
	LOG_INFO << "subscribe [" << channel << "] success!";
}
```

































