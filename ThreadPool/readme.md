# 线程池实现
```c++
// 支持任意函数，但是第一个参数必须是int，表示线程编号
void arg(int idx, int &x) {
    x++;
}

int main() {
    int x = 0;
    tp::ThreadPool tp;
    for(int i=0; i<100000; i++) {
        auto fret = tp.push(arg, x);
    }
    std::cout << "value is: "<< x << std::endl;
    return 0;
}
```