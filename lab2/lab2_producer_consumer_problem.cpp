/**
 * Author: LanSnowZ
 * Date: 2022.2.25
 */

#include "iostream"
#include "thread"
#include "mutex"
#include "condition_variable"

using namespace std;
#define MAX_NUM 1000

mutex mtx;
condition_variable empty_cv, full_cv;
int my_empty = MAX_NUM;
int my_full = 0;

//生产者
// 参考资料: https://www.cnblogs.com/fnlingnzb-learner/p/9542183.html
// unique_lock会在构造时自动上锁, 析构时自动解锁, 更安全方便的管理互斥锁
void Producer(){
    while (1){
        unique_lock<mutex> lock(mtx); //这里是获取锁的过程,使用unique_lock获取锁并自动加锁
        if(my_empty == 0)
            empty_cv.wait(lock); //这里会先释放锁, 等到被唤醒后再自动获取
        my_full++;my_empty--;
        printf("Producer has produced a product, product num: %d\n", my_full);
        full_cv.notify_one();
    }
}

//消费者
void Consumer(){
    while (1){
        unique_lock<mutex> lock(mtx); //这里是获取锁的过程,使用unique_lock获取锁并自动加锁
        if(my_full == 0)
            full_cv.wait(lock); //这里会先释放锁, 等到被唤醒后再自动获取
        my_full--;my_empty++;
        printf("Consumer has taken a product, product num: %d\n", my_full);
        empty_cv.notify_one();
    }
}


//参考资料: https://blog.csdn.net/sjc_0910/article/details/118861539 (多线程std::thread库)
int main(){
    std::thread consumer, producer;
    consumer = thread(Consumer);
    producer = thread(Producer);
    consumer.join();
    producer.join();
    return 0;
}