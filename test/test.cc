//
// Created by 于承业 on 2022/4/4.
//

#include <utility>

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
#include "co/co.h"
#include "co/flag.h"

void example_ringqueue(){
    lfringqueue<int, 8000> rq;
    co::WaitGroup wg;
    auto f = [&rq, &wg](){
        for (int i = 0; i < 1000; ++i) {
            rq.enqueue(new int(i));
        }
        wg.done();
    };
    wg.add(8);
    for(int i=0;i<8;++i) go(f);
    wg.wait();
    printf("Size of rq: %ld\n", rq.countguess());
}

struct Example {
    int num;
    std::string str;
    Example():num(999), str("hello"){}
    Example(int n, std::string s):num(n), str(std::move(s)){}
};

int main(int argc, char** argv) {
    flag::init(argc, argv);

//    auto add1 = [](int x){return x+1;};
//    auto mul2 = [](int x)->int{return (x<<1);};
//    Pipe<int, int> p([](int x){return x*4;});
//    printf("%d\n", 2 | p | add1 | mul2);

//    auto result = std::make_tuple(2, 3, 4) | p | add1 | mul2 ;
//    vector<int>{1, 2, 3} | ranges::filter([](int i){return i%2==0;}) | ranges::take(0)| ranges::transform([](int i){return to_stirng(i);});
//
//    Video --> AVPackage --> PVideo:(from(AVPkgs) --> transform_to_avframe)              --> merge --> RenderV:()
//                        --> PAudio:(from(AVPkgs) --> transform_to_avframe --> resample) -->       --> RenderA:()

    example_ringqueue();


    auto str_to_upper = [](Example* in)->Example*{
        std::transform(in->str.begin(), in->str.end(), in->str.begin(), ::toupper);
        return in;
    };
    auto concat = [](Example* in)->std::string*{
        auto* out = new std::string(in->str + "_" + std::to_string(in->num));
        delete in;
        return out;
    };

    // 1. 定义
    pipeline::_pipe_base pp1, pp2;
    pipeline::Output o;

    // 2. 组合
    pp1.SetNext(&pp2);
    pp2.SetNext(&o);

    // 3. 处理函数
    pp1.SetFunc<Example, Example>(str_to_upper);
    pp2.SetFunc<Example, std::string>(concat);

    pipeline::Segment ppl(&pp1);

    auto *sched = co::next_scheduler();
    co::WaitGroup wg;
    co::Event ev;
    wg.add(2);
    auto push_data = [&ev, &wg, &ppl](){
        for(int i=0;i<1000;++i){
            ppl.Submit(new Example(i, std::string(1, char(i%26+'a'))));
//            ppl.RunOnce();
            ev.signal();
        }
        wg.done();
    };
    auto pop_data = [&wg, &ppl](){
        int cnt = 0;
        std::string *pdata{nullptr};
        while(true){
            ppl.Get(pdata);
            if(pdata != nullptr){
                ++cnt;
                std::cout<<(*pdata)<<std::endl;
            }
            if(cnt==1000) break;
        }
        wg.done();
    };

    ppl.Run(ev);
    go(push_data);
    go(pop_data);


    wg.wait();
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