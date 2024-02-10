#include <iostream>
#include <atomic>
#include <ThreadPool/ThreadPool.h>

void  arg(int idx, int &x) {
    x++;
}

int main() {
    int x = 0;
    tp::ThreadPool tp;
    for(int i=0; i<100000; i++) {
        auto fret = tp.push(arg, x);
        fret.wait();
        if(i==100000/2) {
            tp.stop(true);
            break;
        }
    }
    std::cout << "value is: "<< x << std::endl;
    return 0;
}
