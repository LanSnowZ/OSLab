/**
 * Author: LanSnowZ
 * Date: 2022.2.25
 */
#include "iostream"
#include "thread"
#include "mutex"
#include "condition_variable"

#define TANK_VOLUME 10 // 水缸容量
#define INIT_BUCKETS_NUM 3 // 桶的数量
#define LITTLE_NUM 3
#define OLD_NUM 5

using namespace std;

//同步量
mutex empty_mutex, full_mutex, bucket_count_mtx; // 水缸互斥量和水桶计数互斥量
condition_variable full_cv, room_cv; // 两个条件变量, 水缸中的水 水缸中的空间
int empty_room = TANK_VOLUME; // 水缸剩余可装水数量(桶)
int full = 0; // 水缸中剩余水数量(桶)
//互斥量
mutex well_mutex, tank_mutex; //水桶互斥量 水井互斥量
condition_variable bucket_cv; // 条件变量 水桶
int bucket_left = INIT_BUCKETS_NUM; // 可用水桶数量

void LittleMonk(int id){
    while(1){
        //检查水缸是否有空间
        unique_lock<mutex> empty_lock(empty_mutex);
        while (empty_room == 0)
            room_cv.wait(empty_lock);
        empty_room--;
        empty_lock.unlock();

        //检查并获取桶
        unique_lock<mutex> bucket_count_lock(bucket_count_mtx);
        while (bucket_left == 0)
            bucket_cv.wait(bucket_count_lock);
        bucket_left--;
        bucket_count_lock.unlock();

        //从水井中打水
        unique_lock<mutex> well_lock(well_mutex);
//        cout << "LittleMonk[" << id << "]_GotWater. Time:" << " ." << endl;
        well_lock.unlock();

        //将打来的水导入水缸中并通知
        unique_lock<mutex> tank_lock(tank_mutex);
        unique_lock<mutex> full_lock(full_mutex);
        full++;
        cout << "LittleMonk[" << id << "]_AddedWater. Water left: " << full << endl;
        full_lock.unlock();
        tank_lock.unlock();
        full_cv.notify_one();

        //归还桶并通知等待桶的进程
        bucket_count_lock.lock();
        bucket_left++;
        bucket_count_lock.unlock();
        bucket_cv.notify_one();
    }
}

void OldMonk(int id){
    while(1){
        //检查水缸是否有水
        unique_lock<mutex> full_lock(full_mutex);
        while (full == 0)
            full_cv.wait(full_lock);
        full--;
        cout << "OldMonk[" << id << "]_ReservedWater. Water left: " << full << endl;
        full_lock.unlock();

        //检查并获取桶
        unique_lock<mutex> bucket_count_lock(bucket_count_mtx);
        while (bucket_left == 0)
            bucket_cv.wait(bucket_count_lock);
        bucket_left--;
        bucket_count_lock.unlock();

        //喝水
        unique_lock<mutex> tank_lock(tank_mutex);
        unique_lock<mutex> empty_lock(empty_mutex);
        empty_room++;
        cout << "OldMonk[" << id << "]_DrunkWater. Room left: " << empty_room << endl;
        empty_lock.unlock();
        tank_lock.unlock();
        room_cv.notify_one();

        //归还桶并通知等待桶的进程
        bucket_count_lock.lock();
        bucket_left++;
        bucket_count_lock.unlock();
        bucket_cv.notify_one();
    }
}

int main(){
    thread little[LITTLE_NUM], old[OLD_NUM];
    for (int i = 0; i < LITTLE_NUM; i++)
        little[i] = thread(LittleMonk, i+1);
    for (int i = 0; i < OLD_NUM; i++)
        old[i] = thread(OldMonk, i+1);

    for (int i = 0; i < LITTLE_NUM; i++)
        little[i].join();
    for (int i = 0; i < OLD_NUM; i++)
        old[i].join();
    return 0;
}