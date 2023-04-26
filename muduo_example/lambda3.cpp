/*************************************************************************
    > File Name: lambda2.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 26 Apr 2023 16:42:29 CST
************************************************************************/

#include <iostream>
using namespace std;

//使用lambda表达式仿函数
// class AddNum {
// public:
//     AddNum(int num) : num_(num) {}
//     // int addNum(int x) const {
//     //     return num_ + x;
//     // }
//     int operator()(int x) const {
//         return num_ + x;
//     }
// private:
//     int num_;
// };

int main() {
    //auto add_num = AddNum(10);
    //auto x = add_num.addNum(5);
    //auto x = add_num(5);
    auto add_num = [num = 10](int x) {return num + x;};
    auto x = add_num(5);
    cout << "x = " << x << endl;
    return 0;
}


