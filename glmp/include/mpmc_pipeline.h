//
// Created by 于承业 on 2022/4/15.
//

#ifndef GLMP_MPMC_PIPELINE_H
#define GLMP_MPMC_PIPELINE_H
#include "ringqueue.hpp"
#include "threadpool.hpp"
#include "co/co.h"

namespace pipeline {
#define RING_BUFFER_SIZE 1024
    using AnyPtr = void*;

    class Pipeline;
    class Segment;

    class _instance {
        _instance() = default;
    };

    class _segment{
    public:
        _segment() = default;
        virtual void Submit(){}
        virtual void Get(){}
        virtual void RunOnce(){}
        virtual void Run(){}
    protected:

    };

    class _pipe_base {
        friend class Pipeline;
        friend class Segment;
    public:
        _pipe_base() = default;

        template<typename T>
         void Submit(T* pdata){
             // TODO: maybe failed
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
            // TODO: support lambda expr
            auto fp = std::make_shared<std::function<OUT*(IN*)>>(std::forward<std::function<OUT*(IN*)>>(f));
            auto wrapped_func = [this, fp](){
                IN* pdata{nullptr};
                Get(pdata);
                if(pdata != nullptr) {
                    OUT* _pdata = (*fp)(pdata);
                    _next->Submit<OUT>(_pdata);
                }
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
            _end_proc = false;
        }
    protected:
        lfringqueue<AnyPtr, RING_BUFFER_SIZE> _queue;   // TODO:考虑换成co::Pool
        std::function<void()> _proc;
        bool _end_proc{true};
        _pipe_base* _next{nullptr};
    };

    class Output: public _pipe_base{
        friend class Pipeline;
        friend class _pipe_base;
    public:
        Output() = default;
    };

    class Segment{
        friend class Segment;
    public:
        Segment() = default;
        explicit Segment(_pipe_base* head):_head(head){
                while(head->_next) head = head->_next;
                _tail = head;
        }
        template<typename T>
        void Submit(T* pdata){
            _head->Submit(pdata);
        }
        template<typename T>
        void Get(T*& pdata){
            _tail->Get(pdata);
        }
        void RunOnce(){
            _pipe_base* p = _head;
            if(!p) return;
            while(p->_next){
                p->RunOnce();
                p = p->_next;
            }
        }
        void Run(co::Event& ev){
            go([&ev, this](){
                while(true){
                    RunOnce();
                }
            });
        }
        void Stop(){}

    private:
        _pipe_base* _head{nullptr};
        _pipe_base* _tail{nullptr};
        Segment* _next{nullptr};
    };

    class Pipeline{
        friend class Pipeline;
    public:
        Pipeline() = default;

    private:
        Segment* _head{nullptr};
    };

};

#endif //GLMP_MPMC_PIPELINE_H
