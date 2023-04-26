# hot

---

### 1.using

1. 在 C++ 中 using 用于声明命名空间，使用命名空间也可以防止命名冲突。
2. using还可以用于定义类型的别名（与typedef效果一致，但是语法略有不同），

#### （1）函数指针别名

##### case1：定义基础类型

```cpp
typedef unsigned int uint_t;
using uint_t = unsigned int;
```

声明符也可以包含类型修饰，从而也能由基本数据类型构造出复合类型，可以使用 using <font color='#BAOC2F'>别名声明</font>来定义类型的别名。

##### case2：定义函数指针别名

在定义基础类型时，通过 using 和 typedef 的语法格式发现二者的使用没有太大区别，其不同在于<font color='#BAOC2F'>定义函数指针</font>时的差别：

```cpp
typedef int(*func)(int, double);//使用typedef定义函数指针 其中func为函数指针 int为其返回类型
using func = int(*)(int, double);//使用using定义函数指针 其中func为函数指针 int为其返回类型
```

使用 using 定义函数指针别名十分直观，将别名的名字分离到了左边，别名对应的实际类型放在了右边，可读性更好。

#### （2）模板定义别名

使用 typedef 重定义类似很方便，但是它有一点限制，比如无法重定义一个模板，

比如我们需要以 int 类型为 key 的 map，它可以和很多类型的 value 值进行映射，如果使用 typedef 这样直接定义就非常麻烦：

```cpp
typedef map<int, string> m1;
typedef map<int, int> m2;
typedef map<int, double> m3;
//在这种情况下我们就不自觉的想到了模板：
template <typename T>
typedef map<int, T> type;	// error, 语法错误
```

使用 typename 不支持给模板定义别名，这个简单的需求仅通过 typedef 很难办到，需要添加一个外敷类：

```cpp
#include <iostream>
#include <functional>
#include <map>
using namespace std;

template <typename T>
//定义外敷类
struct MyMap {
    typedef map<int, T> type;
};

int main(void) {
    MyMap<string>::type m;
    m.insert(make_pair(1, "luffy"));
    m.insert(make_pair(2, "ace"));

    MyMap<int>::type m1;
    m1.insert(1, 100);
    m1.insert(2, 200);

    return 0;
}
```

通过上边的例子可以直观的感觉到，需求简单但是实现起来并不容易。

在 C++11 中，新增了一个特性就是可以通过使用 using 来为一个模板定义别名，对于上面的需求可以写成这样：

```cpp
template <typename T>
using mymap = map<int, T>;
```

```cpp
#include <iostream>
#include <functional>
#include <map>
using namespace std;

template <typename T>
using mymap = map<int, T>;

int main(void) {
    // map的value指定为string类型
    mymap<string> m;
    m.insert(make_pair(1, "luffy"));
    m.insert(make_pair(2, "ace"));

    // map的value指定为int类型
    mymap<int> m1;
    m1.insert(1, 100);
    m1.insert(2, 200);

    return 0;
}
```

通过使用 using 给模板指定别名，就可以基于别名非常方便的给 value 指定相应的类型，这样使编写的程序更加灵活，总结：

1. using 语法和 typedef 一样，并不会创建出新的类型，它们只是给某些类型定义了新的别名。
2. using 相较于 typedef 的优势在于定义函数指针别名时<font color='#BAOC2F'>看起来更加直观</font>，并且<font color='#BAOC2F'>可以给模板定义别名</font>。

### 2.可调用对象

- 可调用对象：C++中有一类对象，可以将该类对象<font color='#BAOC2F'>按照函数的方式进行调用</font>，称之为可调用对象。
- 对象包装器：将不同类型的可调用对象，再次进行包装类型统一，以统一的类型进行参数的传递。
- 对象绑定器：给包装好的可调用对象，进行参数绑定（进行初始化操作）。

可调用对象有如下几种定义：

1. 是一个<font color='#BAOC2F'>函数指针</font> 

    ```cpp
    int print(int a, double b) {
        cout << a << b << endl;
        return 0;
    }
    // 定义函数指针
    int (*func)(int, double) = &print;
    using func = void(*)(int, double);
    ```

2. 是一个<font color='#BAOC2F'>具有operator()成员函数</font>的<font color='#BAOC2F'>类对象</font>（仿函数）

    ```cpp
    struct Test {
        void operator()(string msg) {//()操作符重载
            cout << "msg: " << msg << endl;
        }
    };
    
    int main(void) {
        Test test;
        test("我是要成为海贼王的男人!!!");//仿函数
        return 0;
    }
    ```

3. 是一个<font color='#BAOC2F'>可被转换为函数指针</font>的<font color='#BAOC2F'>类对象</font> 

    ```cpp
    using func_ptr = void(*)(int, string);
    
    struct Test {
        operator func_ptr() {//将类对象转换为函数指针
            return print2;//返回函数地址只能是print2(静态函数的函数地址)
        }
        void print1(int a, string b) {
            /* 返回的函数地址只能是print2(静态) 而不能是print1(非静态) */
            /* 这是因为静态方法是属于类的 非静态方法是属于对象的 在还没有定义对象之前这个函数是不存在的 */
            cout << a << b << endl;
        }
        static void print2(int a, string b) {
            cout << a << b << endl;
        }
    };
    
    int main(void) {
        Test test;
        test(19, "Monkey D. Luffy");// 对象转换为函数指针, 并调用
        return 0;
    }
    ```

4. 是一个<font color='#BAOC2F'>类成员函数指针</font>或者<font color='#BAOC2F'>类成员指针</font> 

    ```cpp
    struct Test {
        void print1(int a, string b) {
            cout << a << b << endl;
        }
        static void print2(int a, string b) {
            cout << a << b << endl;
        }
        int m_num;
    };
    
    int main(void) {
        //1.定义类成员函数指针指向类成员函数
        //1-1.指向静态成员函数
        using func_ptr = void(*)(int, string);
        func_ptr f = &Test::print2;
        //1-2.指向非静态成员函数（在1-1的基础上指定函数作用域）
        using func_ptr = void(Test::*)(int, string);
        func_ptr f = &Test::print1;
        void (Test::*func_ptr)(int, string) = &Test::print1;//合并写法
        
        //2.类成员指针指向类成员变量
        using obj_ptr = int Test::*;
        obj_ptr ptr1 = &Test::m_num;
        //int Test::*obj_ptr = &Test::m_num;
        
        //3.通过类对象进行调用操作
        Test test;
        (test.*func_ptr)(19, "Monkey D. Luffy");//通过类成员函数指针调用类成员函数
        test.*obj_ptr = 1;//通过类成员指针初始化类成员变量
        cout << "number is: " << t.m_num << endl;
        return 0;
    }
    ```

在上面的例子中满足条件的这些可调用对象，对应的类型被统称为可调用类型。C++ 中的可调用类型虽然具有比较统一的操作形式，但定义方式非常多，这样在<font color='#BAOC2F'>使用统一的方式保存</font>，或者<font color='#BAOC2F'>传递一个可调用对象</font>时会十分繁琐。

现在C++11通过提供 `std::function` 和 `std::bind` 统一了可调用对象的各种操作。

#### （1）包装器

`std::function` 是可调用对象的包装器(<font color='#BAOC2F'>模板类型的类</font>)，可以容纳<font color='#BAOC2F'>除了类成员/函数指针之外的所有可调用对象</font>（bind绑定器协助）。

通过指定它的<font color='#BAOC2F'>模板参数</font>，它可以用<font color='#BAOC2F'>统一的方式处理函数、函数对象、函数指针</font>，并允许保存和延迟执行它们。

1. 包装 普通函数
2. 包装 类的静态函数
3. 包装 仿函数
4. 包装 可转换为函数指针的对象

##### case1：基本用法

```cpp
#include <functional>
std::function<返回值类型(参数类型列表)> diy_name = 可调用对象;
```

```cpp
int add(int a, int b) {
    cout << a << " + " << b << " = " << a + b << endl;
    return a + b;
}

class T1 {
public:
    static int sub(int a, int b) {
        cout << a << " - " << b << " = " << a - b << endl;
        return a - b;
    }
};

using func_ptr = void(*)(int, string);

class T2 {
public:
    int operator()(int a, int b) {
        cout << a << " * " << b << " = " << a * b << endl;
        return a * b;
    }
    operator func_ptr() {//将类对象转换为函数指针
        return print2;//返回函数地址只能是print2(静态函数的函数地址)
    }
    void print1(int a, string b) {
        /* 返回的函数地址只能是print2(静态) 而不能是print1(非静态) */
        /* 这是因为静态方法是属于类的 非静态方法是属于对象的 在还没有定义对象之前这个函数是不存在的 */
        cout << a << b << endl;
    }
    static void print2(int a, string b) {
        cout << a << b << endl;
    }
};

int main(void) {
    //1.绑定一个普通函数
    function<int(int, int)> f1 = add;
    //2.绑定以静态类成员函数
    function<int(int, int)> f2 = T1::sub;
    //3.绑定一个仿函数
    T2 t;
    function<int(int, int)> f3 = t;
    //4.包装对象转换为函数指针之后 的可调用对象
    T2 t;
    function<void(int, string)> f4 = t;
    
    //函数调用
    f1(9, 3);
    f2(9, 3);
    f3(9, 3);
    f4(9, "string");
    return 0;
}
```

包装完成得到的对象相当于一个<font color='#BAOC2F'>函数指针</font>，和函数指针的使用方式相同，通过<font color='#BAOC2F'>包装器对象</font>就可以完成<font color='#BAOC2F'>对包装的函数的调用了</font>。

##### case2：作为回调函数使用

因为回调函数本身就是通过<font color='#BAOC2F'>函数指针实现的</font>，使用对象包装器可以取代函数指针的作用：

<mark>使用对象包装器优点</mark>：

- 使用对象包装器std::function可<font color='#BAOC2F'>将仿函数转为函数指针</font>，通过函数指针传递在其他函数合适的位置就可以调用包装好的仿函数了。
- 另外使用 std::function <font color='#BAOC2F'>作为函数的传入参数</font>，可以<font color='#BAOC2F'>将定义方式不相同的可调用对象</font>进行<font color='#BAOC2F'>统一的传递</font>，这样大大增加了程序的灵活性。

```cpp
class A {
public:
    /* 构造函数参数是一个包装器对象 */
    A(const function<void()>& f) : callback(f) { }
    void notify() {
        callback(); //调用通过构造函数得到的函数指针
    }
private:
    function<void()> callback;
};

class B {
public:
    void operator()() {
        cout << "我是要成为海贼王的男人!!!" << endl;
    }
};

int main(void) {
    A a(add);
    a.notify(1, 2);
    
    A a(T1::sub);
	a.notify(4, 2);
    
    B b;
    A a(b);//仿函数通过包装器对象进行包装
    a.notify();
    return 0;
}
```

#### （2）绑定器

std::bind用来将<font color='#BAOC2F'>可调用对象与其参数</font>绑定到一起。

绑定后的结果可以使用std::function进行保存，并延迟调用到任何我们需要的时候。其主要有两大作用：

1. 将可调用对象与其参数<font color='#BAOC2F'>绑定成一个仿函数</font>。
2. 将多元（参数个数为n，n>1）可调用对象转换为一元或者（n-1）元可调用对象，即只绑定部分参数。

绑定器函数使用语法格式如下：

```cpp
//绑定非类成员函数/变量
auto f = std::bind(可调用对象地址, 绑定的参数/占位符);
//绑定类成员函/变量
auto f = std::bind(类函数/成员地址, 类实例对象地址, 绑定的参数/占位符);
```

##### case1：绑定非类成员函数\变量

下面来看一个关于绑定器的实际使用的例子：

```cpp
#include <iostream>
#include <functional>
using namespace std;

void callFunc(int x, const function<void(int)>& f) {
    /* 第二个参数为包装器类型 如果要传入实参必须为一个 可调用对象（通过绑定器获得） */
    /* 可以对output_add函数进行绑定 得到一个可调用对象（仿函数） 再将其传递给包装器 */
    /* 如果x的值为偶数 则调用包装器包装得到的 仿函数f(x) 即函数output_add被调用 */
    if (x % 2 == 0) f(x);
}

void output(int x) { cout << x << " "; }

void output_add(int x) { cout << x + 10 << " "; }

int main(void) {
    //1.使用绑定器绑定 可调用对象和参数
    auto f1 = bind(output, placeholders::_1);//绑定成功后得到一个可调用对象（仿函数）
    for (int i = 0; i < 10; ++i) callFunc(i, f1);
    cout << endl;

    auto f2 = bind(output_add, placeholders::_1);
    for (int i = 0; i < 10; ++i) callFunc(i, f2);
    cout << endl;

    return 0;
}
```

在上面的程序中，使用了 std::bind 绑定器，在函数外部通过绑定不同的函数，控制了最后执行的结果。

std::bind绑定器返回的是一个<font color='#BAOC2F'>仿函数类型</font>，得到的返回值可以直接赋值给一个std::function，在使用的时候我们并不需要关心绑定器的返回值类型，使用auto进行自动类型推导就可以了。

##### case2：placeholders占位符

placeholders :: \_1 是一个占位符，代表这个位置将在函数调用时被传入的第一个参数所替代。同样还有其他的占位符 placeholders :: \_2、placeholders :: \_3、placeholders :: \_4、placeholders :: _5 等……

有了占位符的概念之后，使得 std::bind 的使用变得非常灵活：

```cpp
#include <iostream>
#include <functional>
using namespace std;

void output(int x, int y) { cout << x << " " << y << endl; }

int main(void) {
    //使用绑定器绑定可调用对象和参数, 并调用得到的仿函数
    //绑定普通函数的地址 并指定了两个固定的参数
    bind(output, 1, 2)();
    bind(output, placeholders::_1, 2)(10);
    bind(output, 2, placeholders::_1)(10);

    //error, 调用时没有第二个参数
    //bind(output, 2, placeholders::_2)(10);错误写法
    //调用时第一个参数10被吞掉了，没有被使用 使用绑定的值
    bind(output, 2, placeholders::_2)(10, 20);

    bind(output, placeholders::_1, placeholders::_2)(10, 20);
    bind(output, placeholders::_2, placeholders::_1)(10, 20);
    return 0;
}
```

std::bind 可以直接绑定函数的所有参数，也可以仅绑定部分参数。

在绑定部分参数的时候，通过使用 std::placeholders 来决定空位参数将会属于调用发生时的第几个参数。

可调用对象包装器 std::function 是不能实现对 <font color='#BAOC2F'>类成员函数指针</font> 或者 <font color='#BAOC2F'>类成员指针</font> 包装的，但是通过绑定器 std::bind 的配合之后，就可以完美的解决这个问题了。

##### case3：绑定类成员函数\变量

```cpp
#include <iostream>
#include <functional>
using namespace std;

class Test {
public:
    void output(int x, int y) {
        cout <<  x << " " << y << endl;
    }
    int m_number = 100;
};

int main(void) {
    Test t;
    //1.绑定类成员函数
    //仿函数转包装器类型
    function<void(int, int)> f1 = bind(&Test::output, &t, placeholders::_1, placeholders::_2);
    //2.绑定类成员变量(公共)
    //仿函数转为包装器类型
    function<int&(void)> f2 = bind(&Test::m_number, &t);
    
    //3.调用
    f1(520, 1314);
    f2() = 2333;
    cout << "t.m_number: " << t.m_number << endl;
    return 0;
}
```

在用绑定器绑定类成员函数或者成员变量的时候需要将它们<font color='#BAOC2F'>所属的实例对象一并传递到绑定器函数内部</font>。

- f1类型是`function<void(int, int)>`，通过std::bind将类成员函数output地址与对象t绑定，转化为一个<font color='#BAOC2F'>仿函数</font>并<font color='#BAOC2F'>存储到对象f1中</font>
- 使用绑定器绑定的类成员变量`m_number`得到的仿函数被存储到了类型为`function<int&(void)>`的包装器对象f2中，并且可以在需要的时候修改这个成员。其中int是绑定的<font color='#BAOC2F'>类成员的类型</font>并且<font color='#BAOC2F'>允许修改绑定的变量</font>，因此需要<font color='#BAOC2F'>指定为变量的引用</font>，由于没有参数因此参数列表指定为void。



### 3.lambda表达式

lambda 表达式有如下的一些优点：

1. 声明式的编程风格：就地匿名定义目标函数或函数对象，不需要额外写一个命名函数或函数对象。
2. 简洁：避免了代码膨胀和功能分散，让开发更加高效。
3. 在需要的时间和地点实现功能闭包，使程序更加灵活。

#### （1）基本用法

lambda 表达式定义了一个<font color='#BAOC2F'>匿名函数</font>，并且可以<font color='#BAOC2F'>捕获一定范围内的变量</font>。lambda 表达式的语法形式简单归纳如下：

```cpp
[capture](params) opt -> ret {body;};
```

其中 capture 是捕获列表，params 是参数列表，opt 是函数选项，ret 是返回值类型，body 是函数体。

1. 捕获列表 []: 捕获一定范围内的变量
2. 参数列表 (): 和普通函数的参数列表一样，如果没有参数参数列表可以省略不写。

    ```cpp
    auto f = [](){return 1;}	// 没有参数, 参数列表为空
    auto f = []{return 1;}		// 没有参数, 参数列表省略不写
    ```
3. opt 选项， 不需要可以省略

    - mutable: 可以修改按值传递进来的拷贝（注意是能修改拷贝，而不是值本身）
    - exception: 指定函数抛出的异常，如抛出整数类型的异常，可以使用 throw ();

4. 返回值类型：C++11 中lambda 表达式的返回值是通过返回值后置语法来定义的。
5. 函数体：函数的实现，这部分不能省略，但函数体可以为空。

#### （2）捕获列表

lambda 表达式的捕获列表可以捕获一定范围内的变量，具体使用方式如下：

1. [] - 不捕捉任何变量
2. [&] - 捕获外部作用域中所有变量，并作为引用在函数体内使用 (按引用捕获)
3. [=] - 捕获外部作用域中所有变量，并作为副本在函数体内使用 (按值捕获) 拷贝的副本在匿名函数体内部是只读的
4. [=, &foo] - 按值捕获外部作用域中所有变量，按照<font color='#BAOC2F'>引用捕获外部变量</font> foo
5. [bar] - 按值捕获 bar 变量，同时不捕获其他变量
6. [&bar] - 按引用捕获 bar 变量，同时不捕获其他变量
7. [this] - 捕获当前类中的 this 指针
    - 让 lambda 表达式拥有和当前类成员函数同样的访问权限
    - 如果已经使用了 & 或者 =, 默认添加此选项

#### （3）使用细节：

1. 如果在捕捉列表中指定了this，就意味在匿名函数体内部，只能够使用this指针指向的类 中的成员函数 及 成员变量。
2. 使用=来捕捉外部变量，则所有传递的值都是拷贝的方式进行传递，并且值都是只读的属性。如果想要修改这些被拷贝来的变量，则需要在函数体前加上mutable关键字修饰。

```cpp
#include <iostream>
#include <functional>
using namespace std;

class Test {
public:
    void output(int x, int y) {
        auto x1 = [] {return m_number; };            //error 没有捕获外部变量，不能使用类成员 m_number
        auto x2 = [=] {return m_number + x + y; };   //ok 以值拷贝的方式捕获所有外部变量
        auto x3 = [&] {return m_number + x + y; };   //ok 以引用的方式捕获所有外部变量
        auto x4 = [this] {return m_number; };        //ok 捕获this指针，可访问对象内部成员
        auto x5 = [this] {return m_number + x + y; };//error 捕获this指针可访问类内部成员，没有捕获到变量x|y不能访问
        auto x6 = [this, x, y] {return m_number + x + y; };  //ok 捕获 this指针 x y
        auto x7 = [this] {return m_number++; };              //ok 捕获this指针，并且可以修改对象内部变量的值
    }
    int m_number = 100;
};
```

```cpp
int main(void) {
    int a = 10, b = 20;
    auto f1 = [] {return a; };           //error 没有捕获外部变量，因此无法访问变量a
    auto f2 = [&] {return a++; };        //ok 使用引用的方式捕获外部变量，可读写
    auto f3 = [=] {return a; };          //ok 使用值拷贝的方式捕获外部变量，可读
    auto f4 = [=] {return a++; };        //error 使用值拷贝的方式捕获外部变量，可读不能写
    auto f5 = [a] {return a + b; };      //error 使用拷贝的方式捕获了外部变量a，没有捕获外部变量b，因此无法访问变量b
    auto f6 = [a, &b] {return a + (b++); };  //ok 使用拷贝的方式捕获了外部变量a只读，使用引用的方式捕获外部变量b可读写
    auto f7 = [=, &b] {return a + (b++); };  //ok 使用值拷贝的方式捕获所有外部变量以及b的引用，b可读写，其他只读
    return 0;
}
```

在匿名函数内部，需要通过 lambda 表达式的捕获列表控制如何捕获外部变量，以及访问哪些变量。

默认状态下lambda表达式<font color='#BAOC2F'>无法修改通过复制方式捕获外部变量</font>，如果<font color='#BAOC2F'>希望修改这些外部变量</font>，需要<font color='#BAOC2F'>通过引用的方式进行捕获</font>。

#### （4）返回值

很多时候，lambda 表达式的返回值非常明显，因此在C++11中允许省略 lambda 表达式的返回值。

```cpp
//完整的lambda表达式定义
auto f = [](int a) -> int {
    return a+10;
};
//忽略返回值的lambda表达式定义
auto f = [](int a) {
    return a+10;
};
```

一般情况下，不指定 lambda 表达式的返回值，编译器会根据 return 语句自动推导返回值的类型，但需要注意的是 labmda表达式<font color='#BAOC2F'>不能通过列表</font>初始化<font color='#BAOC2F'>自动推导出返回值类型</font>。

```cpp
//ok 可以自动推导出返回值类型
auto f = [](int i) { 
    return i;
}
//error 不能推导出返回值类型
auto f1 = []() {
    return {1, 2};//基于列表初始化推导返回值，错误
}
```

#### （5）函数本质

使用lambda表达式捕获列表捕获外部变量，如果希望去修改按值捕获的外部变量，那么应该如何处理呢？这就需要使用 mutable 选项，被mutable修饰是lambda表达式<font color='#BAOC2F'>就算没有参数也要写明参数列表</font>，并且<font color='#BAOC2F'>可以去掉按值捕获的外部变量的只读</font>（const）属性。

```cpp
int a = 0;
auto f1 = [=] {return a++; };              // error, 按值捕获外部变量, a是只读的
auto f2 = [=]()mutable {return a++; };     // ok
```

什么通过值拷贝的方式捕获的外部变量是只读的？

1. lambda表达式的类型在C++11中会被看做是<font color='#BAOC2F'>一个带operator()的类</font>，即<font color='#BAOC2F'>仿函数</font>。
2. 按照C++标准lambda表达式的<font color='#BAOC2F'>operator()默认是const的</font>，一个const成员函数是无法修改成员变量值的。mutable 选项的作用就在于取消 operator () 的 const 属性：

因为lambda表达式在C++中会被看做是一个仿函数，因此可以使用std::function和std::bind来<font color='#BAOC2F'>存储和操作lambda表达式</font>：

```cpp
#include <iostream>
#include <functional>
using namespace std;

int main(void) {
    //1.包装可调用函数
    std::function<int(int)> f1 = [](int a) {return a; };
    //2.绑定可调用函数
    std::function<int(int)> f2 = bind([](int a) {return a; }, placeholders::_1);

    //3.函数调用
    cout << f1(100) << endl;
    cout << f2(200) << endl;
    return 0;
}
```

- 若lambda 表达式没有捕获任何的外部变量 ，则其可以<font color='#BAOC2F'>转换成一个普通的函数指针</font>：


```cpp
using func_ptr = int(*)(int);
//没有捕获任何外部变量的匿名函数
func_ptr f = [](int a) {
    return a;
};
//函数调用
f(1314);
```

#### （6）使用案例：

```cpp
//案例1：输出两数之和
auto f = [](int a, int b) -> int { return a + b; };
cout << f(1, 2) << endl;
```

```cpp
//案例2：对vec数组进行升序sort排序
vector<int> vec {0, 11, 3, 19, 22, 7, 1, 5};
auto f = [](int a, int b) { return a < b; };
sort(vec.begin(), vec.end(), f);
```

```cpp
//案例3-1：加法成员函数
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
```

```cpp
//案例3-2：加法仿函数
#include <iostream>
using namespace std;

//使用仿函数替代成员函数
class AddNum {
public:
    AddNum(int num) : num_(num) {}
    int operator()(int x) const { return num_ + x; }
private:
    int num_;
};

int main() {
    auto add_num = AddNum(10);
    auto x = add_num(5);
    cout << "x = " << x << endl;
    return 0;
}
```

```cpp
//案例3-2：加法lambda表达式
#include <iostream>
using namespace std;

//使用lambda表达式仿函数
int main() {
    auto add_num = [num = 10](int x) {return num + x;};//lambda表达式替换整个AddNum类
    auto x = add_num(5);
    cout << "x = " << x << endl;
    return 0;
}
```

其实lambda表达式也是一个特殊的类类型，只不过不知道现在该类型的名字，编译器会帮其取名。













