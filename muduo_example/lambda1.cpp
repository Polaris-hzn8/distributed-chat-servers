/*************************************************************************
    > File Name: lambda1.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 26 Apr 2023 16:42:29 CST
************************************************************************/

#include <iostream>
using namespace std;

//使用成员函数调用
class AddNum {
public:
    AddNum(int num) : num_(num) {}
    int addNum(int x) const { return num_ + x; }
private:
    int num_;
};

int main() {
    auto add_num = AddNum(10);
    auto x = add_num.addNum(5);
    cout << "x = " << x << endl;
    return 0;
}
