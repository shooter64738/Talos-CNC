#ifndef _C_SYS_GPIO_CONFIG_
#define _C_SYS_GPIO_CONFIG_

#include <stdint.h>

static uint32_t dummy;

#define STEPPER_PUL_GPIO_ENABLER
#define STEPPER_PUL_PORT dummy
#define STEPPER_PUL_PORT_DIRECT_REGISTER dummy
#define STEPPER_PUL_PIN_0 0
#define STEPPER_PUL_PIN_1 0
#define STEPPER_PUL_PIN_2 0
#define STEPPER_PUL_PIN_3 0
#define STEPPER_PUL_PIN_4 0
#define STEPPER_PUL_PIN_5 0
#define STEPPER_PUL_PORT_MASK	( 1<<STEPPER_PUL_PIN_0 \
								| 1<<STEPPER_PUL_PIN_1 \
								| 1<<STEPPER_PUL_PIN_2 \
								| 1<<STEPPER_PUL_PIN_3 \
								| 1<<STEPPER_PUL_PIN_4 \
								| 1<<STEPPER_PUL_PIN_5 )


#define STEPPER_DIR_GPIO_ENABLER 
#define STEPPER_DIR_PORT dummy
#define STEPPER_DIR_PORT_DIRECT_REGISTER dummy
#define STEPPER_DIR_PIN_0 1
#define STEPPER_DIR_PIN_1 1
#define STEPPER_DIR_PIN_2 1
#define STEPPER_DIR_PIN_3 1
#define STEPPER_DIR_PIN_4 1
#define STEPPER_DIR_PIN_5 1
#define STEPPER_DIR_PORT_MASK	( 1<<STEPPER_DIR_PIN_0 \
								| 1<<STEPPER_DIR_PIN_1 \
								| 1<<STEPPER_DIR_PIN_2 \
								| 1<<STEPPER_DIR_PIN_3 \
								| 1<<STEPPER_DIR_PIN_4 \
								| 1<<STEPPER_DIR_PIN_5 )


static void step_pulse_config(void)
{
}
#endif