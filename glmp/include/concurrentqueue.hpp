//
// Created by 于承业 on 2022/4/9.
//

#ifndef GLMP_CONCURRENTQUEUE_HPP
#define GLMP_CONCURRENTQUEUE_HPP
#include<atomic>
#include<stdexcept>

/**
 * 链表无锁队列
 */
template<typename T>
class LinkedConcurrentQueue{
    struct Node{
        T val;
        std::atomic<Node*> next;
        explicit Node(const T& v):val(v), next(nullptr){}
        template<typename ...Args>
        explicit Node(Args&& ...args):val(std::forward<Args>(args)...), next(nullptr){}
    };
public:
    LinkedConcurrentQueue()= default;
    /**
     * 根据参数原地构造对象；或者被push(T&& elem)调用，进行复制构造
     * @tparam ... 构造对象所用参数的类型
     * @param args 构造对象所用参数
     */
    template<typename ...Args>
    void emplace(Args&& ...args){
        Node* new_node{new Node(std::forward<Args>(args)...)};
        Node* old_back{nullptr};
        do{
            old_back = _back.load();
        } while(old_back->next.compare_exchange_weak(nullptr, new_node) != true);
        _back.compare_exchange_weak(old_back, new_node);
        if(++_size==1) _front.compare_exchange_weak(nullptr, new_node);
        ++_capacity;
    }
    /**
     * 插入左值时被调用，存储的是复制构造后的对象
     * @param elem
     */
    void push(const T& elem){
        Node* new_node{new Node(elem)};
        Node* old_back{nullptr};
        do{
            old_back = _back.load();
        } while(old_back->next.compare_exchange_weak(nullptr, new_node) != true);
        _back.compare_exchange_weak(old_back, new_node);
        if(++_size==1) _front.compare_exchange_weak(nullptr, new_node);
        ++_capacity;
    }
    /**
     * @brief 插入右值时被调用，存储的是复制构造后的对象
     * @param elem 插入的元素
     */
    void push(T&& elem){
        emplace(std::move(elem));
    }
    /**
     * @brief 队首元素出队
     */
    void pop(){
        if(_size==0) throw std::runtime_error("Queue is empty");
        Node* old_front{_front.load()};
        while(_front.compare_exchange_weak(old_front, old_front->next) != true);
        --_size;
    }
    /**
     * 队首元素出队并返回其引用
     * @return 队首元素的引用
     */
    T& deque(){
        if(_size==0) throw std::runtime_error("Queue is empty");
        Node* old_front{_front.load()};
        while(_front.compare_exchange_weak(old_front, old_front->next) != true);
        --_size;
        return old_front->val;
    }
    /**
     * @brief 释放已出队元素的内存空间
     * @param n 最多保留的未释放元素个数
     */
    void reserve(size_t n){
        Node* old_head{nullptr};
        size_t old_capacity = _capacity.load();
        size_t old_size = _size.load();
        while(n>old_size && old_capacity>n){
            do{
                old_head = _head.load();
            }while(_head.compare_exchange_weak(old_head, old_head->next));
            delete old_head;
            --n;
            --_capacity;
        }
    }
    /**
     * @brief 返回队首元素的引用
     * @return 队首元素的引用
     */
    T& front(){
        return _front.load()->val;
    }
    /**
     * @return 队列中的元素数量
     */
    size_t size(){
        return _size.load();
    }
    /**
     * @return 队列是否为空
     */
    bool empty(){
        return _size.load()==0;
    }
private:
    std::atomic<size_t> _size{0};
    std::atomic<size_t> _capacity{0};
    std::atomic<Node*> _head{nullptr}; // 链表头元素
    std::atomic<Node*> _front{nullptr}; // 队首元素
    std::atomic<Node*> _back{nullptr}; // 队尾元素

};

#endif //GLMP_CONCURRENTQUEUE_HPP
