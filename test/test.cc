//
// Created by 于承业 on 2022/4/4.
//

#include "glfw3.h"
#include "include/library.h"
#include "include/pipeline.h"
#include "cstdio"
#include "memory"
#include "vector"

int main(){
    auto add1 = [](int x){return x+1;};
    auto mul2 = [](int x)->int{return (x<<1);};
    Pipe<int, int> p([](int x){return x*4;});
    printf("%d\n", 2|p|add1|mul2);
}