/*************************************************************************
    > File Name: main.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 10:27:18 CST
************************************************************************/

#include "head.h"
#include "sys.h"
#include "common.h"
#include "data.h"
#include "Group.h"
#include "User.h"
#include "imfunc.h"
#include "subthread.h"

void print();
int serverConnect(char *ip, uint16_t port);
void login(int clientfd);
void regis(int clientfd);
void quit(int clientfd);

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

    //2.连接服务器
    int clientfd = serverConnect(ip, port);

    //int sem_init (sem_t *__sem, int __pshared, unsigned int __value)
    sem_init(&rwsem, 0, 0);

    //连接服务器成功后 立即启动 数据接收子线程
    //启动客户端消息接受线程 不停的接受来自服务端的所有数据
    /* pthread_create & pthread_detach */
    thread readTask(readTaskHandler, clientfd);//创建线程用于服务器消息接受
    readTask.detach();//设置分离线程 防止线程被join 线程执行完成内核中PCB资源可能会泄露 设置成分离线程 线程运行结束PCB自动回收

    //3.主线程用于接收用户输入 负责发送数据
    for (;;) {
        // 进入系统登录界面
        print();
        // 为什么要读取调缓冲区的回车？
        // 前面读入了整数int 后面需要读入字符串 如果整数不读调缓冲区残留的回车 则会面的字符串读取将直接把回车读掉
        // 当从缓冲区读入整数后 需要及时将缓冲区残留的回车读掉 防止后面读取字符串出现问题
        int choice; cin >> choice; cin.get();//读调缓冲区残留的回车

        switch(choice) {
            case 1:
                login(clientfd);//用户登录
                break;
            case 2:
                regis(clientfd);//用户注册
                break;
            case 3:
                quit(clientfd);//关闭登录界面
            default:
                cerr << "invalid input!" << endl;
                break;
        }
    }
    return 0;
}


void print() {
    cout << "=======================" << endl;
    cout << "1.login" << endl;
    cout << "2.register" << endl;
    cout << "3.quit" << endl;
    cout << "=======================" << endl;
    cout << "choice:";
}

int serverConnect(char *ip, uint16_t port) {
    //1.创建socketfd
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        cerr << "socketfd create error!~" << endl;
        exit(-1);
    }

    //2.进行连接
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    if (connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in)) == -1) {
        cerr << "connect server error!~" << endl;
        exit(-1);
    }
    return clientfd;
}


void login(int clientfd) {
    int uid = 0;
    char password[50] = {0};
    cout << "uid:"; cin >> uid; cin.get();
    cout << "password:"; cin.getline(password, 50);

    //1.组装json数据
    json js;
    js["msgId"] = LOGIN_MSG;
    js["uid"] = uid;
    js["password"] = password;
    string request = js.dump();

    //2.发送json request 数据
    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send login request error!~" << request << endl;

    //3.等待recv子线程处理操作 返回login是否成功
    isLoginSuccess_g = false;
    sem_wait(&rwsem);

    if (isLoginSuccess_g) {
        //4.recv响应消息后 若登录成功 则进入聊天主菜单页面 用户可以开始进行各种业务操作
        isMainMenuRunning_g = true;
        mainMenu(clientfd);
    }
}

void regis(int clientfd) {
    char username[50] = {0};
    char password[50] = {0};
    cout << "username:"; //cin >> userid; cin.get(); 不能使用cin遇见空格结束 否则输入的内容不能带有空格 gets
    cin.getline(username, 50);
    cout << "passwored: "; //cin >> passwd; cin.get();
    cin.getline(password, 50);

    //1.组装json数据
    json js;
    js["msgId"] = REG_MSG;
    js["username"] = username;
    js["password"] = password;
    string request = js.dump();

    //2.发送json request 数据
    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send reg request error!~" << request << endl;
    
    //4.等待子线程处理操作完成 主线程再继续操作
    sem_wait(&rwsem);
}

void quit(int clientfd) {
    close(clientfd);
    sem_destroy(&rwsem);
    exit(0);
}




