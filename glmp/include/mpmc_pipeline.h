//
// Created by 于承业 on 2022/4/15.
//

#ifndef GLMP_MPMC_PIPELINE_H
#define GLMP_MPMC_PIPELINE_H
#include "ringqueue.hpp"

namespace pipeline {
#define RING_BUFFER_SIZE 128
    using AnyPtr = void*;

    class _instance {
        _instance() = default;


    };

    class _pipe_base {
    public:
        _pipe_base() = default;

        template<typename T>
        void Submit(T* pdata){
            auto* p = new AnyPtr(pdata);
            _queue.enqueue(p);
        }

        template<typename T>
        void Submit(const T& elem){
            T* pdata = new T(elem);
            _queue.enqueue(new AnyPtr(pdata));
        }

        template<typename T>
        void Submit(T&& elem){
            T* pdata = new T(elem);
            _queue.enqueue(new AnyPtr(pdata));
        }

        template<typename T>
        void Get(T*& pdata){
            AnyPtr* p = nullptr;
            _queue.dequeue(&p);
            pdata = (p==nullptr) ? nullptr : static_cast<T*>(*p);
        }

        template<typename IN, typename OUT>
        void SetFunc(std::function<OUT*(IN*)>&& f){
            auto fp = std::make_shared<std::function<OUT*(IN*)>>(f);
            auto wrapped_func = [this, fp](){
                IN* pdata = nullptr;
                Get(pdata);
                OUT* _pdata = (*fp)(pdata);
                _next->Submit<OUT>(_pdata);
            };
            _proc = wrapped_func;
        }

        void RunOnce(){
            _proc();
        }

        /**
         * Run is an async mathod that continuously Get data from _queue, process data, and Submit to _next pipe
         */
        void Run(){
            // TODO
        }

        void SetNext(_pipe_base* next){
            _next = next;
        }
    private:
        lfringqueue<AnyPtr, RING_BUFFER_SIZE> _queue;
        std::function<void()> _proc;
        _pipe_base* _next{nullptr};
    };

    class Pipeline{
    public:
        Pipeline() = default;
        void run(){}
    private:
        _pipe_base* head{nullptr};
    };

};

#endif //GLMP_MPMC_PIPELINE_H
