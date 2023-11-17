#include "ticker.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <unistd.h>
#endif

void sleep_ms(unsigned ms)
{
	#ifdef WIN32
	Sleep(ms);
	#elif _POSIX_C_SOURCE >= 199309L
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
	#else
	usleep(ms * 1000);
	#endif
}
void sleep_after_func_ms(void (*func)(), unsigned ms)
{
	struct timespec ts_beg, ts_end;
	clock_gettime(CLOCK_MONOTONIC, &ts_beg);
	func();
	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	unsigned after_ms = timespec_to_ms(ts_end) - timespec_to_ms(ts_beg);
	if(after_ms < ms)
		sleep_ms(ms - after_ms);
}

static void* __ticker_thread(void* args)
{
	ticker* t = (ticker*)args;
	struct timespec ts_beg, ts_end;
	while(t->state == TICKER_STATE_RUNNING){
		clock_gettime(CLOCK_MONOTONIC, &ts_beg);
		t->tick_func(t->tick_delay_ms);
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
		unsigned ms = timespec_to_ms(ts_end) - timespec_to_ms(ts_beg);
		if(ms < t->tick_delay_ms)
			sleep_ms(t->tick_delay_ms - ms);
	}
	return NULL;
}

ticker* ticker_create(unsigned tick_delay_ms, tick_func_t tick_func)
{
	ticker* t = malloc(sizeof(ticker));
	t->tick_func = tick_func;
	t->tick_delay_ms = tick_delay_ms;
	t->state = 1;
	pthread_create(&t->thread, NULL, __ticker_thread, t);
	return t;
}
void ticker_free(ticker* t)
{
	t->state = TICKER_STATE_STOPPED;
	free(t);
}

/* Commons tickers */

static ticker* __ticker_ui;
static llist_func __ticker_ui_funcs;

static void __tick_func_ui(unsigned ms)
{
	llist_func_node* n = NULL;
	while((n = llist_func_next(&__ticker_ui_funcs, n)))
		n->data(ms);
}
llist_func_node* ui_ticker_add(tick_func_t tick_func)
{
	return llist_func_insert_begin(&__ticker_ui_funcs, tick_func);
}

void tickers_init()
{
	__ticker_ui = ticker_create(16, __tick_func_ui);
	llist_func_create(&__ticker_ui_funcs);
}
