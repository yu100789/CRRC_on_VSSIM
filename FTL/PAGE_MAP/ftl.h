// File: ftl.h
// Date: 18-Sep-2017
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2017
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#ifndef _FTL_H_
#define _FTL_H_

#include "common.h"

extern FILE* fp_w_event;
extern FILE* fp_ch_util;
extern FILE* fp_wb_lat;
extern FILE* fp_rw_lat;
typedef struct LPN_INFO
{
    int64_t lpn;
    unsigned int offset;
    struct LPN_INFO *prev;
    struct LPN_INFO *next;
    pthread_mutex_t lock;
}LPN_INFO;
typedef struct LPN_FREQ
{
    int64_t lpn;
    unsigned int freq;
    struct LPN_FREQ *prev;
    struct LPN_FREQ *next;
}LPN_FREQ;
typedef struct page_buffer
{
	void* addr;
	unsigned int n_empty_space;
	void* ftl_ptr;
	void* host_ptr;
	void* buffer_end;
	int is_full;		// 1: full, 0: others
    int is_empty;
    struct LPN_INFO *head;
    struct LPN_INFO *tail;
    pthread_mutex_t lock;
    pthread_cond_t ready;
}page_buffer;
typedef struct HOT_LRU
{
    uint64_t length;
    struct LPN_FREQ *head;
    struct LPN_FREQ *tail;
    pthread_mutex_t lock;
}HOT_LRU;


/*************************
  Define Variables 
 *************************/
/* ACCESS_TYPE */
#define HOT_W_THRESHOLD 255
#define HOT_R_THRESHOLD 255
#define cWcR		0
#define cWhR_head		1
#define cWhR_tail		2
#define hWcR		3
#define hWhR		4



void FTL_INIT(void);
void FTL_TERM(void);

int FTL_READ(int core_id, uint64_t sector_nb, uint32_t length);
int FTL_WRITE(volatile int core_id, volatile uint64_t sector_nb, volatile uint32_t length,volatile void* buf);
void FTL_DISCARD(int core_id, uint64_t sector_nb, uint32_t length);

int _FTL_READ(int core_id, uint64_t sector_nb, uint32_t length);
int _FTL_WRITE(int core_id, uint64_t sector_nb, uint32_t length);
int _FTL_WRITE_WITH_PAGE_BUF(int core_id, uint64_t sector_nb, uint32_t length);

unsigned long int GET_TOTAL_WRITE_PAGES(void);
unsigned long int GET_TOTAL_READ_PAGES(void);
uint64_t GET_TOTAL_WRITE_LPN(void);
uint64_t GET_TOTAL_READ_LPN(void);
uint32_t GET_TOTAL_5LC(void);
uint32_t GET_TOTAL_6LC(void);
uint32_t GET_TOTAL_7LC(void);
uint32_t GET_TOTAL_8LC(void);
double GET_ENTROPY(void);
double GET_CR(void);
void INIT_PAGE_BUFFER(void);
int INSERT_LPN_TO_PAGE_BUFFER(int buffer_nb,LPN_INFO *cur);
void FLUSH_PAGE_BUFFER(int buffer_nb,int core_id);
void INIT_HOT_LRU(void);
void insert_lru(LPN_FREQ * new_lpn,int type);
int check_lru_index(int64_t lpn,int type);
int get_DataAccessType(int64_t lpn);
uint32_t recalculate_length(uint64_t sector_nb, uint32_t length);
// LPN_FREQ* insertionSortList(LPN_FREQ* head);
// LPN_FREQ* reverseList(LPN_FREQ* head);
#endif
