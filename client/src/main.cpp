/*************************************************************************
    > File Name: main.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 10:27:18 CST
************************************************************************/

#include "head.h"
#include "Group.h"
#include "User.h"
#include "common.h"

//将部分服务端部分消息在客户端进行保存 当客户端需要使用时可以直接使用
//减轻访问服务器次数 减轻服务器压力
//登录的用户信息
User userInfo_g;
//登录用户的好友列表
vector<User> friendList_g;
//登录用户的群组信息列表
vector<Group> groupList_g;

//显示当前登录用户的基本信息
void showUserInfo();
//主聊天页面程序
void mainMenu();

//获取系统时间
string getCurrentTime();
//消息接收线程
void readTaskHandler(int clientfd);
//消息接收线程
void writeTaskHandler(int clientfd);

//main线程用作发送线程 子线程用作接收线程
//聊天客户端程序实现
int main(int argc, char *argv[]) {
    //1.命令行参数获取
    if (argc < 3) {
        cerr << "command invalid! example: ./myClient ip port" << endl;
        exit(-1);
    }
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    //2.创建socketfd
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        cerr << "socketfd create error!~" << endl;
        exit(-1);
    }

    //3.进行连接
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    if (connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in)) == -1) {
        cerr << "connect server error!~" << endl;
        exit(-1);
    }

    //4.主线程用于接收用户输入 负责发送数据
    for (;;) {
        // 4-1 系统首页面
        cout << "=======================" << endl;
        cout << "1.login" << endl;
        cout << "2.register" << endl;
        cout << "3.quit" << endl;
        cout << "=======================" << endl;
        cout << "choice:";

        /*
            为什么要读取调缓冲区的回车？
            前面读入了整数int 后面需要读入字符串 如果整数不读调缓冲区残留的回车 则会面的字符串读取将直接把回车读掉
            当从缓冲区读入整数后 需要及时将缓冲区残留的回车读掉 防止后面读取字符串出现问题
        */

        int choice;
        cin >> choice;
        cin.get();//读调缓冲区残留的回车

        switch(choice) {
            case 1: {
                /* 用户登录 */
                int uid = 0;
                char password[50] = {0};
                cout << "uid:"; cin >> uid; cin.get();
                cout << "passwored: "; cin.getline(password, 50);

                /* 组装json数据 */
                json js;
                js["msgId"] = LOGIN_MSG;
                js["uid"] = uid;
                js["password"] = password;
                string request = js.dump();

                /* 发送json request 数据 */
                int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
                if (len == -1) cerr << "send login request error!~" << request << endl;
                else {
                    /* 接受json response 数据 */
                    char buff[1024] = {0};
                    if ((recv(clientfd, buff, 1024, 0)) == -1) cerr << "recv login response error!~" << request << endl;
                    else {
                        json response = json::parse(buff);
                        if (response["errno"].get<int>() != 0) {
                            /* 登录信息有误 打印服务器返回的异常信息 */
                            cerr << response["errmsg"] << endl;
                        } else {
                            /* 登录成功 初始化本地数据 */
                            /* 记录登录用户的uid与username */
                            userInfo_g.setId(response["uid"].get<int>());
                            userInfo_g.setName(response["username"]);

                            /* 记录当前用户的好友列表信息 */
                            if (response.contains("friends")) {
                                vector<string> friends = response["friends"];
                                for (string friend_s : friends) {
                                    json friend_j = json::parse(friend_s);
                                    User friend_;
                                    friend_.setId(friend_j["uid"]);
                                    friend_.setName(friend_j["username"]);
                                    friend_.setState(friend_j["state"]);
                                    friendList_g.push_back(friend_);
                                }
                            }

                            /* 记录当前用户的群组列表信息 */
                            if (response.contains("groups")) {
                                vector<string> groups = response["groups"];
                                for (string group_s : groups) {
                                    json group_j = json::parse(group_s);
                                    Group group_;
                                    group_.setId(group_j["gid"]);
                                    group_.setName(group_j["groupname"]);
                                    group_.setDesc(group_j["groupdesc"]);

                                    vector<string> groupusers = response["groupusers"];
                                    for (string groupuser_s : groupusers) {
                                        json groupuser_j = json::parse(groupuser_s);
                                        GroupUser groupuser_;
                                        groupuser_.setId(groupuser_j["uid"]);
                                        groupuser_.setName(groupuser_j["username"]);
                                        groupuser_.setState(groupuser_j["state"]);
                                        groupuser_.setRole(groupuser_j["grouprole"]);

                                        vector<GroupUser> groupusers_;
                                        groupusers_.push_back(groupuser_);
                                    }

                                    groupList_g.push_back(group_);
                                }
                            }

                            /* 显示当前登录用户的基本信息 */
                            cout << userInfo_g.getName() << " login success, userid is " << userInfo_g.getId() << " ." << endl;
                            showUserInfo();

                            /* 显示当前用户的离线消息（私聊离线信息 & 群组离线消息） */
                            if (response.contains("offlinemsg")) {
                                vector<string> offlinemsgs = response["offlinemsg"];
                                for (string offlinemsg_s : offlinemsgs) {
                                    json offlinemsg_j = json::parse(offlinemsg_s);
                                    int uid = offlinemsg_j["uid"];
                                    string username = offlinemsg_j["username"];
                                    string msg = offlinemsg_j["msg"];
                                    string time = offlinemsg_j["time"];
                                    printf("<%s-%s-%d> : %s", time.c_str(), username.c_str(), uid, msg.c_str());
                                }
                            }

                            /* 启动客户端消息接受线程 不停的接受来自服务端的所有数据 */
                            thread readTask(readTaskHandler, clientfd);//创建线程用于服务器消息接受
                            readTask.detach();//设置分离线程 防止线程被join 线程执行完成内核中PCB资源可能会泄露 设置成分离线程 线程运行结束PCB自动回收

                            /* 进入聊天主菜单页面 用户可以开始进行各种业务操作 */
                            mainMenu();

                        }
                    }
                }
            }

            case 2: {
                /* 用户注册 */
                char username[50] = {0};
                char password[50] = {0};
                cout << "username:"; //cin >> userid; cin.get(); 不能使用cin遇见空格结束 否则输入的内容不能带有空格 gets
                cin.getline(username, 50);
                cout << "passwored: "; //cin >> passwd; cin.get();
                cin.getline(password, 50);

                /* 组装json数据 */
                json js;
                js["msgId"] = REG_MSG;
                js["username"] = username;
                js["password"] = password;
                string request = js.dump();

                /* 发送json request 数据 */
                int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
                if (len == -1) cerr << "send reg request error!~" << request << endl;
                else {
                    /* 接受json response 数据 */
                    char buff[1024] = {0};
                    if ((recv(clientfd, buff, 1024, 0)) == -1) cerr << "recv reg response error!~" << request << endl;
                    else {
                        json response = json::parse(buff);
                        if (response["errno"].get<int>() != 0) {
                            cerr << username << " is already exist, please change your username!~" << request << endl;
                        } else {
                            cout << username << " register success, userid is " << response["uid"] << ", don't forget it!" << endl;
                        }
                    }
                }
            }

            case 3: {
                /* 用户登出 */
                close(clientfd);
                exit(0);
            }

            default: {
                cerr << "invalid input!" << endl;
                break;
            }
        }
    }
    return 0;
}


//显示当前登录用户的基本信息
void showUserInfo() {

}

//主聊天页面程序
void mainMenu() {

}

//获取系统时间
string getCurrentTime() {

}

//消息接收线程
void readTaskHandler(int clientfd) {

}

//消息接收线程
void writeTaskHandler(int clientfd) {

}




