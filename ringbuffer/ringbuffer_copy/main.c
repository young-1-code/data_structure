/**
 * 简介：测试循环buffer
 * 时间：2023/3/1
 * 作者：小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/

#include <stdio.h>
#include "ringbuffer.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define ENABLE_WRITE_OUT_FILE 0

static volatile int is_runing = 0;

long get_sys_time(void)                         //获取系统时间
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
    return tm.tv_sec*1000000+tm.tv_usec;
}

void* write_data_task(void* args)               //线程1 往循环buffer中写数据
{
    FILE *fp = fopen("../ringbuffer_cpp/testfile/src.dat", "rb");
    if(!fp){
        printf("open src.dat error!\n");
        is_runing=0;
        return NULL;
    }
    uint8_t buf[1024]={0};
    ringbuffer_t *rb = (ringbuffer_t*)args;     //获取传入进来的循环buffer参数
    int ret = -1;
    int data_len = 0;

    while(!feof(fp))
    {

        data_len = 512+rand()%512;              //获取随机长度写入循环buffer
        fread(buf, data_len, 1, fp);            //根据长度从文件中读出原始数据写入循环buffer
        do{
            ret = rb->write(rb, buf, data_len); //往循环buffer中写数据
        }while(ret == -1);                      //阻塞等待写入成功
    }
    is_runing=0;

    fclose(fp);
    return NULL;
}

void* read_data_task(void* args)
{
    #if ENABLE_WRITE_OUT_FILE
    FILE *fp = fopen("./data/out.dat", "wb");

    if(!fp){
        printf("Open out.dat error! \n");
        is_runing=0;
        return NULL;
    }
    #endif
    int ret = -1;
    ringbuffer_t *rb = (ringbuffer_t*)args;     //获取传入进来的循环buffer参数
    uint8_t buf[1024]={0};
    
    long start_time = get_sys_time();           //获取系统时间
    int data_len=0;
    while (is_runing)
    {
        data_len = 512+rand()%512;              //获取随机长度从循环buffer中读取数据
        do{
            ret = rb->read(rb, buf, data_len);  //从循环buffer中读数据
        }while(ret==-1);                        //阻塞等待读取数据成功
        #if ENABLE_WRITE_OUT_FILE
        fwrite(buf, data_len, 1, fp);           //将从循环buffer中读取的数据写入文件
        #endif
    }
    long end_time = get_sys_time();             //获取系统时间
    int use_time = end_time-start_time;
    double use_s = ((double)use_time/1000000.0);
    double rate = ((500*1024*1024*8.0)/use_s)/(1024*1024*1024.0);
    printf("500M Data Use time=%dus(%.2lfS) rate=%.2lfGbps\n", use_time, use_s, rate);

    #if ENABLE_WRITE_OUT_FILE
    fclose(fp);
    #endif
    return NULL;
}

int main(int argc, char** argv)
{
    pthread_t th[2]={0};
    ringbuffer_t* rb = creat_ringbuffer(16*1024);       //创建循环buffer

    srand(time(NULL));                                  //初始化随机数
    is_runing = 1;
    
    pthread_create(&th[0], NULL, write_data_task, rb);  //创建线程写数据
    pthread_create(&th[1], NULL, read_data_task, rb);   //创建线程读数据

    pthread_join(th[0], NULL);                          //阻塞等待线程执行完毕
    pthread_join(th[1], NULL);                          //创建线程写数据

    printf("Finish Test Ringbuffer...\n");

    destory_ringbuffer(rb);                             //销毁循环buffer

    return 0;
}
