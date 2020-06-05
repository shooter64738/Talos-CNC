#ifndef _C_SYS_TIMER_CONFIG_
#define _C_SYS_TIMER_CONFIG_

//static uint32_t dummy;

#define STEP_TIMER uint32_t dummy1
#define STEP_TIMER_CLK_ENABLE 
#define STEP_TIMER_INTERRUPT uint32_t dummy2
#define STEP_TIMER_DELAY_DIRECT_REGISTER uint32_t dummy3
#define STEP_TIMER_COUNT_DIRECT_REGISTER uint32_t dummy4

#define STEP_RST_TIMER uint32_t dummy5
#define STEP_RST_TIMER_CLK_ENABLE
#define STEP_RST_TIMER_INTERRUPT uint32_t dummy6
#define STEP_RST_TIMER_DELAY_DIRECT_REGISTER uint32_t dummy7
#define STEP_RST_TIMER_COUNT_DIRECT_REGISTER uint32_t dummy8

#define LOWEST_STEP_DELAY_VALUE 10

static void step_timer_config(void)
{

}
#endif