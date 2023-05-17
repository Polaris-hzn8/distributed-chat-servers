# 并发聊天服务器
---

### 一、主要技术点：

Jason开发库、boost + muduo网络库、redis环境、mysql环境

使用muduo网络库，将网络IO代码与业务代码分离，只需要关注业务代码接口（用户的断开与连接、用户的可读写事件）

1. Json序列化与反序列化
2. muduo网络库开发
3. redis缓存服务器编程
4. 基于发布订阅模式的服务器中间件redis消息队列
5. Mysql数据库
6. 网络IO模块、业务模块、数据模块分层设计

### 二、项目主要功能：

1. 用户注册
2. 用户登录
3. 添加好友、删除好友
4. 添加群组、删除群组
5. 好友聊天、
6. 群组聊天

### 三、并发聊天服务器分层架构

1. 连接层entry/gate：为客户端收发消息提供出入口，主要作用包括：

    - 保持海量用户连接，

    - 解析协议对传输内容进行编码，

    - 维护session

    - 推送消息
2. 核心业务层logic：负责IM系统各项功能的核心逻辑实现
3. 路由层route：负责消息投递
4. 数据层data：负责IM系统相关数据的持久化存储，包括消息内容、账号信息等
5. 外部接口层：常用的第三方系统推送服务，苹果手机自带的APNs服务、安卓手机内置的谷歌公司GCM服务等
6. 基础组件：

<mark>网络服务器编程常用模型</mark>：

1. 方案1：accept + read/write（非并发服务器）
2. 方案2：accept + fork（process pre connection）
3. 方案3：accept + thread（thread pre connection）
4. 方案4：reactors in threads （one loop per thread）：有一个main reactor负载accept连接，然后将连接分发到某个sub reactor（采用round-robin的方式来选择sub reactor），该连接的所有操作都在sub reactor所处的线程中完成，多个连接可能被分派到多个线程中，以充分利用CPU。
5. reactors in process（one loop per process）：nginx服务器的网络模块设计，基于进程设计采用多个Reacors充当IO进程和工作进程，通过accept锁解决多个Reactors的惊群现象。

### 四、数据库表设计

#### 1.User表

| 字段名称 | 字段类型                  | 字段说明     | 约数                        |
| -------- | ------------------------- | ------------ | --------------------------- |
| id       | INT                       | 用户id       | PRIMARY KEY、AUTO_INCREMENT |
| name     | VARCHAR(50)               | 用户名       | NOT NULL, UNIQUE            |
| password | VARCHAR(50)               | 用户密码     | NOT NULL                    |
| state    | ENUM('online', 'offline') | 当前登录状态 | DEFAULT 'offline'           |

#### 2.friend表

用户与用户之间的关系表：

| 字段名称 | 字段类型 | 字段说明 | 约数               |
| -------- | -------- | -------- | ------------------ |
| userid   | INT      | 用户id   | NOT NULL、联合主键 |
| friendid | INT      | 好友id   | NOT NULL、联合主键 |

#### 3.AllGroup表

| 字段名称  | 字段类型     | 字段说明   | 约数                        |
| --------- | ------------ | ---------- | --------------------------- |
| id        | INT          | 组id       | PRIMARY KEY、AUTO_INCREMENT |
| groupname | VARCHAR(50)  | 组名称     | NOT NULL,UNIQUE             |
| groupdesc | VARCHAR(200) | 组功能描述 | DEFAULT ''                  |

#### 4.GroupUser表

用户与群组之间的关系表：

| 字段名称  | 字段类型                  | 字段说明 | 约数               |
| --------- | ------------------------- | -------- | ------------------ |
| groupid   | INT                       | 群组id   | NOT NULL、联合主键 |
| userid    | INT                       | 群组员id | NOT NULL、联合主键 |
| grouprole | ENUM('creator', 'normal') | 组内角色 | DEFAULT ‘normal’   |

#### 5.OfflineMessage表

离线消息表：

| 字段名称 | 字段类型     | 字段说明                      | 约数     |
| -------- | ------------ | ----------------------------- | -------- |
| userid   | INT          | 用户id                        | NOT NULL |
| message  | VARCHAR(500) | 离线消息（内容为 Json字符串） | NOT NULL |



### 五、运行

运行环境：

1. mysql、mysql-dev环境、source chat.sql
2. redis、hiredis（可选）
3. nginx负载均衡配置（可选）

执行命令：

```shell
mkdir build
cd build
cmake ..
make
```





