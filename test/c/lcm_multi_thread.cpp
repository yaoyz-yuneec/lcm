
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <lcm/lcm.h>

#include "common.h"
#include "pthread_pool.h"


#define info(...)             \
    do {                      \
        printf("c_client: "); \
        printf(__VA_ARGS__);  \
        printf("\n");         \
    } while (0)

static lcm_t *g_lcm = NULL;

// ================================= echo test
const int pthread_num = 10;
const int g_msg_data_len = 512;
int g_echo_response_count[pthread_num] = {0};


typedef struct md_lcm_msg{
    int msg_id;
    char msg_data[g_msg_data_len];
}md_lcm_msg_t;
md_lcm_msg_t g_msg[pthread_num];

TEST(LCM_C, CreateLCM)
{
    g_lcm = lcm_create(NULL);
    int i,j;
    for(i = 0; i < pthread_num; i++){
        g_msg[i].msg_id = i;
        for(j = 0; j < g_msg_data_len; j++){
            g_msg[i].msg_data[j] = rand()%256;
        }
    }
}

static void echo_handler(const lcm_recv_buf_t *rbuf, const char *channel, void *user)
{
    md_lcm_msg_t *p = (md_lcm_msg_t *)user;
    md_lcm_msg_t *p2 = (md_lcm_msg_t *)rbuf->data;
    if (rbuf->data_size != sizeof(md_lcm_msg_t)){
        info("rbuf->data_size: %d, msg_len: %d", rbuf->data_size, (int)sizeof(md_lcm_msg_t));
        return;
    }

    if(p->msg_id == p2->msg_id){
        if(memcmp(p, p2, rbuf->data_size)){
            printf("[receive] msg id cur: %d rep: %d --mismathch content\n",
                    p->msg_id, p2->msg_id);
            return;
        }
    }else{
        printf("[receive] msg id cur: %d rep: %d --mismath ID\n",
                p->msg_id, p2->msg_id);
        return;
    }
    printf("[receive] msg id cur: %d rep: %d --success\n",
            p->msg_id, p2->msg_id);
    g_echo_response_count[p->msg_id]++;
}

void lcm_publish_func(void *arg)
{

    md_lcm_msg_t *p = (md_lcm_msg_t *)arg;
    lcm_subscription_t *subs = lcm_subscribe(g_lcm, "TEST_ECHO_REPLY", echo_handler, arg);
    g_echo_response_count[p->msg_id] = 0;

    int iter;
    for (iter = 0; iter < 8; iter++) {

        //int j;
        //for(j = 0; j < g_msg_data_len; j++){
            //p[p->msg_id].msg_data[j] = rand()%256;
        //}
        //usleep(100);
        lcm_publish(g_lcm, "TEST_ECHO", p, sizeof(md_lcm_msg_t));

        printf("[send]msg id: %d lcm_publish %d iter\n", p->msg_id, iter);
        ASSERT_GT(lcm_handle_timeout(g_lcm, 500), 0);
        //ASSERT_EQ(g_echo_response_count[p->msg_id], iter + 1);
        //printf("[send-after-success]msg id: %d lcm_publish %d iter\n", p->msg_id, iter);

        //if (g_echo_response_count[p->msg_id] != iter + 1) {
            //info("echo test failed to receive response on iteration %d", iter);
            //lcm_unsubscribe(g_lcm, subs);
            //return;
        //}
    }

    lcm_unsubscribe(g_lcm, subs);
    return;
}

TEST(LCM_C, MultiThreadLCM)
{
    ASSERT_TRUE(g_lcm != NULL);

    void* p = pool_start(lcm_publish_func, pthread_num);
    int i = 0;

    for(i = 0; i < 8; i++){
        pool_enqueue(p, (void *)(g_msg+i), 0);
    }

    pool_wait(p);

}
