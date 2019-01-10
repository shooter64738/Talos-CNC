/*
 * ARM_Coordinator.cpp
 *
 * Created: 11/21/2018 8:00:28 PM
 * Author : jeff_d
 */ 


#include "sam.h"
#include "..\Shared\c_processor.h"



int main(void)
{
    /* Initialize the SAM system */
    SystemInit();

    c_processor::startup();
}
