/*
* c_motion_core_arm_3x8e_inputs.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_motion_core_arm_3x8e_inputs.h"
#include "../../../helpers.h"
#include "../../../MotionDriver/c_processor.h"
volatile uint32_t reads;
uint8_t Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms = 0;

void Hardware_Abstraction_Layer::MotionCore::Inputs::initialize()
{
	
	
	
	configure_input(PIOB,MOTOR_DRIVER_ALARM_X_AXIS); //arduio due pin 2
	configure_input(PIOC,MOTOR_DRIVER_ALARM_Y_AXIS); //arduio due pin 3
	configure_input(PIOC,MOTOR_DRIVER_ALARM_Z_AXIS); //arduio due pin 4
	configure_input(PIOC,MOTOR_DRIVER_ALARM_A_AXIS); //arduio due pin 5
	configure_input(PIOC,MOTOR_DRIVER_ALARM_B_AXIS); //arduio due pin 6
	configure_input(PIOC,MOTOR_DRIVER_ALARM_C_AXIS); //arduio due pin 7
	configure_input(PIOC,MOTOR_DRIVER_ALARM_U_AXIS); //arduio due pin 8
	configure_input(PIOC,MOTOR_DRIVER_ALARM_V_AXIS); //arduio due pin 9
	//configure_input(PIOC,PIO_PC29B_TIOB7); //arduio due pin 10
	//configure_input(PIOD,PIO_PD7B_TIOA8);  //arduio due pin 11
	//configure_input(PIOD,PIO_PD8B_TIOB8);  //arduio due pin 12
	//configure_input(PIOB,PIO_PB27B_TIOB0); //arduio due pin 13
	
	NVIC_EnableIRQ(PIOB_IRQn);NVIC_SetPriority(PIOB_IRQn, 0);
	NVIC_EnableIRQ(PIOC_IRQn);NVIC_SetPriority(PIOC_IRQn, 0);
	//NVIC_EnableIRQ(PIOD_IRQn);NVIC_SetPriority(PIOD_IRQn, 0);
	PMC->PMC_PCER0 |= (1 << ID_PIOB) | (1 << ID_PIOC);// | (1 << ID_PIOD);
}

void  Hardware_Abstraction_Layer::MotionCore::Inputs::configure_input(Pio * Port, uint32_t Pin)
{
	/*
	PIO_PER  - write 1's here to override other peripherals and allow GPIO use for pins
	PIO_OER  - write 1's here to set pins as OUTPUT
	PIO_ODR  - write 1's here to set pins as INPUT
	* PIO_SODR   - write 1's here to set output pins HIGH
	* PIO_CODR   - write 1's here to set output pins LOW
	* PIO_PDSR  - read's actual state of the pins on the port.
	PIO_PUDR  - write 1's here to switch off internal pull-up for pins
	PIO_PUER  - write 1's here to switch on internal pull-up for pins
	*/
	//Enable interrupts
	Port->PIO_CODR |=Pin;
	Port->PIO_PER |=Pin;
	Port->PIO_OER |=Pin;
	Port->PIO_ODR |=Pin;
	//Port->PIO_OER |=Pin;
	Port->PIO_PUER |=Pin;
	//Enable additional interrupt mode
	
	//Port->PIO_SCIFSR |=Pin;
	//Port->PIO_DIFSR |=Pin;
	//Port->PIO_IFDGSR |=Pin;
	//Port->PIO_SCDR |=Pin;
	
	//Set to edge detection
	Port->PIO_ESR |= Pin;
	//Rising edge
	Port->PIO_REHLSR |= Pin;
}
void PIOB_Handler()
{
	uint32_t status = PIOB->PIO_ISR;
	
	if ((status & MOTOR_DRIVER_ALARM_X_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_X_AXIS);		
	}
}
void PIOC_Handler()
{
	uint32_t status = PIOC->PIO_ISR;
	
	if ((status & MOTOR_DRIVER_ALARM_Y_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_Y_AXIS);		
	}
	
	if ((status & (MOTOR_DRIVER_ALARM_Z_AXIS)))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_Z_AXIS);
	}
	
	if ((status & MOTOR_DRIVER_ALARM_A_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_A_AXIS);
	}
	
	if ((status & MOTOR_DRIVER_ALARM_B_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_B_AXIS);
	}

	if ((status & MOTOR_DRIVER_ALARM_C_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_C_AXIS);
	}

	if ((status & MOTOR_DRIVER_ALARM_U_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_U_AXIS);
	}

	if ((status & MOTOR_DRIVER_ALARM_V_AXIS))
	{
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms |= (1<<MACHINE_V_AXIS);
	}
}
void PIOD_Handler()
{
	//if ((PIOD->PIO_ISR & PIO_PD7B_TIOA8) && (PIOD->PIO_PDSR & PIO_PD7B_TIOA8))
	//{
	//}
	//
	//if ((PIOD->PIO_ISR & PIO_PD8B_TIOB8) && (PIOD->PIO_PDSR & PIO_PD8B_TIOB8))
	//{
	//}
}

