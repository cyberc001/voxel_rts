#ifndef TICKER_H
#define TICKER_H

#include <pthread.h>
#include <stdatomic.h>
#include "linked_list.h"

typedef void (*tick_func_t)(unsigned ms);

#define TICKER_STATE_STOPPED	0
#define TICKER_STATE_RUNNING	1

typedef struct {
	unsigned tick_delay_ms; // CAN BE CHANGED 
	pthread_t thread; // DO NOT CHANGE AT ALL
	atomic_int state; // CAN BE CHANGED TO STOP RUNNING THREAD, BETTER USE ticker_free()
	tick_func_t tick_func; // AVOID CHANGING DUE TO RACE CONDITIONS
} ticker;

#define timespec_to_ms(ts) (((ts).tv_sec * 1000) + ((ts).tv_nsec / 1000000))
void sleep_ms(unsigned ms);
void sleep_after_func_ms(tick_func_t tick_func, unsigned ms);

ticker* ticker_create(unsigned tick_delay_ms, tick_func_t tick_func);
void ticker_free(ticker* t);

/* Common tickers */
DEF_LINKED_LIST(llist_func, tick_func_t)
void tickers_init();

llist_func_node* ui_ticker_add(tick_func_t tick_func);

#endif
