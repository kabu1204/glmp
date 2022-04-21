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
#include "sys/ucontext.h"
#include "include/mpmc_pipeline.h"
//#include "include/dynamic_pipeline.h"

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

struct test_s {
    int a;
    std::string s;
    test_s():a(999), s("hello"){}
};

int main(){

//    auto add1 = [](int x){return x+1;};
//    auto mul2 = [](int x)->int{return (x<<1);};
//    Pipe<int, int> p([](int x){return x*4;});
//    printf("%d\n", 2 | p | add1 | mul2);

//    auto result = std::make_tuple(2, 3, 4) | p | add1 | mul2 ;
//    vector<int>{1, 2, 3} | ranges::filter([](int i){return i%2==0;}) | ranges::take(0)| ranges::transform([](int i){return to_stirng(i);});
//
//    Video --> AVPackage --> PVideo:(from(AVPkgs) --> transform_to_avframe)              --> merge --> RenderV:()
//                        --> PAudio:(from(AVPkgs) --> transform_to_avframe --> resample) -->       --> RenderA:()

    pipeline::_pipe_base pt1;
    pipeline::_pipe_base pt2;
    pt1.SetNext(&pt2);
    auto proc = [](test_s* in)->int*{
        int* out = new int(in->a+1);
        return out;
    };
    test_s a;
    a.s = "world";
    pt1.Submit(new test_s());
    pt1.Submit(&a);
    pt1.SetFunc<test_s, int>(proc);
    pt1.RunOnce();
    for(int i=0;i<2;++i) {
        test_s *pdata = nullptr;
        pt1.Get(pdata);
        if(pdata) std::cout<<pdata->s<<" ";
    }

    int* intp = nullptr;
    pt2.Get(intp);
    if(intp) std::cout<<*intp<<std::endl;


//    QueuePipeline<bool, short, int, double> q;
//    q.DebugInfo();
//
//    Pipeline<int, long, float, double> P;
//    int data0 = 10;
//    int *data1 = &data0;
//    int && data2 = 20;  // data2本身是个左值，引用了右值
//    double *result {nullptr};
//
//    // 方式一，阻塞
//    P.Submit(data0);
//    P.Submit(data1);
//    P.Submit(data2);
//    P.Submit(30);
//
//    // 方式二，非阻塞
//    P.AsyncSubmit(data0);   // const int &
//    P.AsyncSubmit(data1);   // int*
//    P.AsyncSubmit(data2);   // const int &
//    P.AsyncSubmit(30);  // int&& -> Emplace
//    P.AsyncSubmit(new int(40)); // int*
//
//    // 方式三，非阻塞
//    P << data0 << data1 << data2 << 30;
//
//
//    for(int i=0;i<4;++i){
//        P.Get(result);
//        //或 P >> result;
//        if(result) printf("%.3f\n", *result);
//    }

//    example_ringqueue();
}