#ifndef _COMMON_
#define _COMMON_


void system_enter_critical(void);
void system_exit_critical(void);

/*不同功能的循环检测功能统一用LOOP_DETECT_REGISTER注册，可搜索关键字看例子*/
struct loop_detect_handler {
    int time;/*TIMER周期的倍数。timer周期默认一般是2ms */
    void (*fun)();
};
extern struct loop_detect_handler loop_detect_handler_begin[];
extern struct loop_detect_handler loop_detect_handler_end[];
#define LOOP_DETECT_REGISTER(handler) \
	const struct loop_detect_handler handler \
		sec_used(.loop_detect_region)

#define list_for_each_loop_detect(h) \
	for (h=loop_detect_handler_begin; h<loop_detect_handler_end; h++)


//maskrom
void delay(unsigned int);
extern void wdt_clear(void);
void irq_init();


#endif
