/*************************************************************************
	> File Name: chatservice.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Sun 30 Apr 2023 04:43:26 PM CST
 ************************************************************************/

/**
 * chatservice 业务层
*/

#ifndef _CHATSERVICE_H
#define _CHATSERVICE_H

#include "head.h"
#include "UserModel.h"
#include <mutex>

//采用单例模式
//将消息id msg_id 与事件TcpConnectionPtr &conn 进行绑定
//使用using为已经存在的类型起别名 MsgHandler事件处理器
//表示处理消息的事件 回调方法类型
using MsgHandler = function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

//采用单例模式设计聊天服务类
//主要工作：为msg_id映射事件回调 1个消息id映射1个事件处理
class ChatService {
public:
	//1.获取单例对象的接口函数
	static ChatService* instance();
	//2.获取消息对应的事件处理器
	MsgHandler getHandler(int msg_id);
	//3.处理登录业务
	void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
	//4.处理注册业务
	void regis(const TcpConnectionPtr &conn, json &js, Timestamp time);
	//5.处理客户端异常退出
	void clientCloseUnexpectedly(const TcpConnectionPtr &conn);

	//6.一对一聊天
	void singleChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
	//7.群聊
	void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
	
private:
	//单例模式
	ChatService();

	//存储消息id 及其对应的事件处理方法表
	unordered_map<int, MsgHandler> _msgHandlerMap;

	//存储在线用户的通信连接（注意线程安全问题）
	mutex _connMutex;//定义互斥锁 保证_userConnMap的线程安全问题
	unordered_map<int, TcpConnectionPtr> _userConnMap;

	//数据操作类对象
	UserModel _userModel;
};

#endif

