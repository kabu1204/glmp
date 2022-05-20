//
// Created by 于承业 on 2022/4/4.
//

#include <utility>

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

//DEF_main(argc, argv){
int main(int argc, char** argv) {
    flag::init(argc, argv);
    co::init();

    example_ringqueue();


    auto str_to_upper = [](Example* in)->Example*{
        std::transform(in->str.begin(), in->str.end(), in->str.begin(), ::toupper);
        return in;
    };
    std::string delim = "?";
    auto concat = [delim](Example* in)->std::string*{
        auto* out = new std::string(in->str + delim + std::to_string(in->num));
        delete in;
        return out;
    };

    auto nf = [](){int* a = new int(0); return a;};
    pipeline::From ppf(nf, 1);

    // 1. 定义
    pipeline::_pipe_base pp1, pp2;
    pipeline::Output o;


    // 2. 组合
    pp1.SetNext(&pp2);
    pp2.SetNext(&o);

    // 3. 处理函数
    pp1.SetFunc(str_to_upper);
    pp2.SetFunc(concat);

    pipeline::Segment ppl(&pp1);


    auto sched_v = co::all_schedulers();

#define PUSH_ROUTINE 0
#define POP_ROUTINE 1

    co::WaitGroup wg;
    wg.add(2);
    auto push_data = [&wg, &ppl, &sched_v](){
        for(int i=0;i<1000;++i){
            ppl.Submit(new Example(i, std::string(1, char(i%26+'a'))));
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

    std::cout<<"PID:"<<getpid()<<std::endl;
    sched_v[PUSH_ROUTINE]->go(push_data);
    sched_v[POP_ROUTINE]->go(pop_data);


    ppl.Run(false);
    wg.wait();
    ppl.Stop();
    printf("End of main\n");
}