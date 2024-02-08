#include <iostream>
#include <atomic>
#include <ThreadPool/ThreadPool.h>

void  arg(int idx, int &x) {
//    std::cout<<"this is "<<idx<<" with "<<x<<std::endl;
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
