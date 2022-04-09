//
// Created by 于承业 on 2022/4/9.
//

#ifndef GLMP_PIPELINE_H
#define GLMP_PIPELINE_H
#include "threadpool.hpp"
#include "vector"
#include "atomic"
#include "future"
#include "mutex"
#include "condition_variable"
#include "thread"
#include "queue"
#include "type_traits"
#include "cstdio"

/**
 * 管道类，每个管道和一个处理函数绑定
 * @tparam OUT_T 该节管道处理结果的类型
 * @tparam IN_T 该节管道输入数据的类型
 */
template<typename OUT_T, typename... IN_T>
class Pipe{
public:
    /**
     * @brief 创建一节对数据进行处理的管道
     * @details
     */
    explicit Pipe(std::function<OUT_T(IN_T...)>&& f):func(f){};
    void process(IN_T... args){
        result = func(std::forward<IN_T...>(args...));
    }
    /**
     * [语法糖]重载管道运算符
     * @tparam NEXT_OUT_T 下一节管道的输出类型
     * @param p 下一节管道
     * @return 参数p的引用
     */
    template<class NEXT_OUT_T>
    Pipe<NEXT_OUT_T, OUT_T>& operator|(Pipe<NEXT_OUT_T, OUT_T>&& p){
        p.process(result);
        return p;
    }
    OUT_T operator()(IN_T... args){
        process(std::forward<IN_T...>(args...));
        return result;
    }

private:
    std::function<OUT_T(IN_T...)> func;
    OUT_T result;
};

class Pipeline{
    /**
     * @brief 创建一条管线（流水线），由一节或多节管道组成
     * @details 每个Pipeline对象仅仅是一条管线，内部的并行是指对数据进行并行处理，而不是多条管线并行。
     * 需要多条管线并行，你需要使用更高的封装：
     */
    Pipeline();
};

//template<typename IN_T, typename OUT_T>
//auto operator|(IN_T&& data, Pipe<OUT_T, IN_T>&& p){
//    p.process(std::forward<IN_T>(data));
//    return std::forward<decltype(p)>(p);
//}

/**
 * [语法糖]重载管道运算符
 * @tparam T 待处理的数据类型
 * @tparam F 函数类型
 * @param data 待处理数据
 * @param f 普通函数、lambda函数、std::function等
 * @return 处理结果
 */
template<typename T, typename F>
auto operator|(T&& data, F&& f){
    return std::forward<F>(f)(std::forward<T>(data));
}


template<typename OUT_T, typename... IN_T>
auto make_pipe(std::function<OUT_T(IN_T...)>&& f){
    return Pipe<OUT_T, IN_T...>(f);
}

#endif //GLMP_PIPELINE_H
