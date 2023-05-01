/*************************************************************************
    > File Name: json.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 21 Apr 2023 18:08:21 CST
************************************************************************/

#include "../include/json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
using namespace std;

void func1() {
    /* 类似unordered_map */
    json js;
    js["msg_type"] = 2;
    js["from"] = "luochenhao";
    js["to"] = "medarin";
    js["content"] = "hello, this is a json test.";
    string str = js.dump();
    cout << str.c_str() << endl;
}

void func2() {
    json js;
    //添加key->value
    js["id"] = {1, 2, 3, 4, 5, 6};
    //添加数组
    js["name"] = "luochenhao";
    //key -> value(key, value)
    js["msg"]["zhangsan"] = "hello world";
    js["msg"]["luochenhao"] = "hello china";
    //一次性添加数组对象
    js["msg"] = {{"zhangsan", "helloworld"}, {"luochenhao", "hello china"}};
    cout << js << endl;
}

void func3() {
    json js;
    //1.直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    //2.直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js << endl;
}

int main() {
    func1();
    func2();
    func3();
    return 0;
}

