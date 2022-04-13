//
// Created by 于承业 on 2022/4/4.
//

#include "glfw3.h"
#include "include/library.h"
#include "include/pipeline.h"
#include "include/concurrentqueue.hpp"
#include "include/ringqueue.hpp"
#include "cstdio"
#include "memory"
#include "vector"
#include "atomic"

void example_ringqueue(){
    lfringqueue<int, 1000> rq;
    for(int i=0;i<100;++i){
        rq.enqueue(new int(i));
    }
    for(int i=0;i<100;++i){
        int *p;
        rq.dequeue(&p);
        printf("%d\n",*p);
    }
}

int main(){
    auto add1 = [](int x){return x+1;};
    auto mul2 = [](int x)->int{return (x<<1);};
    Pipe<int, int> p([](int x){return x*4;});
    printf("%d\n", 2 | p | add1 | mul2);

    QueuePipeline<bool, short, int, double> q;
    q.DebugInfo();

    Pipeline<int, long, float, double> P;
    int data0 = 10;
    int *data1 = &data0;
    int && data2 = 20;  // data2本身是个左值，引用了右值
    double *result {nullptr};

    // 方式一，阻塞
    P.Submit(data0);
    P.Submit(data1);
    P.Submit(data2);
    P.Submit(30);

    // 方式二，非阻塞
    P.AsyncSubmit(data0);   // const int &
    P.AsyncSubmit(data1);   // int*
    P.AsyncSubmit(data2);   // const int &
    P.AsyncSubmit(30);  // int&& -> Emplace
    P.AsyncSubmit(new int(40)); // int*

    // 方式三，非阻塞
    P << data0 << data1 << data2 << 30;


//    for(int i=0;i<4;++i){
//        P.Get(result);
//        //或 P >> result;
//        if(result) printf("%.3f\n", *result);
//    }

//    example_ringqueue();
}