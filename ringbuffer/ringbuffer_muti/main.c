/**
 * 简介：测试循环buffer
 * 时间：2023/9/14 修改:2024/4/25
 * 作者：小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include "ringbuffer_muti.h"

volatile int is_runing=0;

m_ringbuffer_t *g_rb;

#define ENABLE_WRITE_OUT_FILE 0 //读取数据是否写文件

long get_sys_time(void)                         //获取系统时间
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
    return tm.tv_sec*1000000+tm.tv_usec;
}

void* write_data_task(void* args)               //线程1 往循环buffer中写数据
{
    FILE *fp = fopen("../src.dat", "rb");
    if(!fp){
        printf("open src.dat error!\n");
        is_runing=0;
        return NULL;
    }
    uint8_t buf[1024]={0};
    m_ringbuffer_t *rb = (m_ringbuffer_t*)args;     //获取传入进来的循环buffer参数
    int ret = -1;
    int data_len = 0;

    while(!feof(fp))
    {
        data_len = 512+rand()%512;              //获取随机长度写入循环buffer
        fread(buf, data_len, 1, fp);            //根据长度从文件中读出原始数据写入循环buffer
        do{
            ret = rb->write(rb, buf, data_len); //往循环buffer中写数据
        }while(ret != 0);                       //阻塞等待写入成功
    }
    is_runing=0;

    fclose(fp);
    return NULL;
}


void* read_data_task(void* args)
{
    int read_id = *(int*)args;

    #if ENABLE_WRITE_OUT_FILE
    char filename[64]={0};
    sprintf(filename, "out_%d.dat", read_id);
    FILE *fp = fopen(filename, "wb");

    if(!fp){
        printf("Open out.dat error! \n");
        is_runing=0;
        return NULL;
    }
    #endif

    m_ringbuffer_t *rb = g_rb;     //获取传入进来的循环buffer参数
    uint8_t *buf=NULL;
    int ret_len=0;
    
    long start_time = get_sys_time();           //获取系统时间
    int data_len=0;
    while (is_runing)
    {
        data_len = 512+rand()%512;              //获取随机长度从循环buffer中读取数据
        do{
            ret_len = rb->read(rb, read_id, &buf, data_len); //从循环buffer中读数据
        }while(ret_len<0 && is_runing);                      //阻塞等待读取数据成功
        #if ENABLE_WRITE_OUT_FILE
        fwrite(buf, ret_len, 1, fp);            //将从循环buffer中读取的数据写入文件
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

int main(void)
{
    int i, a_id[64];
	pthread_t wth, th[64];
	const int test_read_cnt = 2;
    m_ringbuffer_t* rb = create_muti_ringbuffer(50*1024);
    g_rb = rb;

	
	srand(time(NULL));                                  //初始化随机数
    is_runing = 1;
	
	pthread_create(&wth, NULL, write_data_task, rb);

    for(i=0; i<test_read_cnt; ++i){
        a_id[i] = rb->add(rb);
    }

    for(i=0; i<test_read_cnt; ++i){
	    pthread_create(&th[i], NULL, read_data_task, &a_id[i]);
    }

    pthread_join(wth, NULL);
    for(i=0; i<test_read_cnt; ++i){
	    pthread_join(th[i], NULL);
    }

    destory_muti_ringbuffer(rb);
	
	printf("Finish Test Ringbuffer...\n");

	return 0;
}
