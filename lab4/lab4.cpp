//
// Created by LanSnowZ on 2023/2/22.
//

#include <iostream>

int test(int *b){
    int *a;
    a = b+1;
    int c = *a;
    a++;
    int d = *a;
    return d;
}

int main(){
    int a[5] = {0,1,2,3,4};
    std::cout << test(a);
    return 0;
}