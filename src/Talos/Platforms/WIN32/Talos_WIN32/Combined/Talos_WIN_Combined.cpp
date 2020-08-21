
#include "../../../../Coordinator/Processing/Main/Coordinator_Process.h"
#include "../../../../Motion/Processing/Main/Motion_Process.h"
#include "../../../../Shared_Data/Kernel/Base/c_kernel_base.h"
#include "../../../../talos_hardware_def.h"

using namespace Talos;
void run(void);
int main2(int argc, char* argv[]);

uint32_t test;
#define PORTD test
#define STEP_PIN 0
#define DIRECTION_PIN 1

uint32_t current_direction = 4;

int main(void)
{
	test = test ^ (1 << DIRECTION_PIN);
	//outputfilename[AvailableTime][DesPos][StrtSpeed][SpeedLimit][AccLimit]
	//int main2(int argc, char* argv[])
	char * args[7];
	args[1] = "test.txt"; //<--60 seconds
	args[2] = "1000"; //<--1 seconds
	args[3] = "1600"; //<--move 5mm, 320 steps per mm
	args[4] = "0"; //<--start at zero speed
	args[5] = "3200000"; //< --max speed is 10,000mm/s 320 steps per mm
	args[6] = "32000"; //<--100mm/s max accel, 320 steps per mm
	main2(4, args);

	//Start the 'core'. 
	//this will configure the board (unless you are running via windows)
	//so that needed peripherals and io is ready.
	Hardware_Abstraction_Layer::Core::initialize();

	Kernel::Base::f_initialize();
	
	//init framework comms (not much going on in here yet)
	Kernel::Comm::f_initialize();

	//init framwork cpus (assign an ID number to each cpu object. Init the data buffers
	Kernel::CPU::f_initialize(
		HOST_CPU_ID, CORD_CPU_ID, MACH_CPU_ID, SPDL_CPU_ID, PRPH_CPU_ID, Hardware_Abstraction_Layer::Core::cpu_tick_ms);
	Kernel::Comm::print(0, "Hello\r\n");
	Coordinator::Main_Process::cord_initialize();
	Motion::Main_Process::mot_initialize();

	Kernel::Comm::host_ring_buffer.put("g1x100\r\n\0");

	run();
}

bool reload = false;
char test_line[50] = "g0x30\r\n\0";
void run(void)
{
	uint8_t state = 0;

	while (state == 0)
	{
		if (reload)
			Kernel::Comm::host_ring_buffer.put(test_line);

		Coordinator::Main_Process::cord_run();
		Motion::Main_Process::mot_run();
	}
}



/*
	step.c
	Program to verify new algorithm for linear acceleration.
	Author: Pramod Ranade <pramod.ranade@spjsystems.com>
*/

#include <stdio.h>
#if defined(MSVC) //|| defined(_WIN64)
#include <io.h>
#endif
#include <stdlib.h>

#define SCAN_TIME_IN_MICRO_SEC	  	2
#define COUNT_FOR_COMPARISON		(1000000000UL / SCAN_TIME_IN_MICRO_SEC)

#define	SPEED_LIMIT					9500
#define	ACC_LIMIT					900000
#define	START_SPEED					1500
#define	DES_POS						117
#define AVAILABLE_TIME_IN_MILI_SEC	23

int OutputPinStatus = 1;
int g_nRisingEdgeCount = 0;
FILE* fptr;

#define	MakeOutputHigh()	OutputPinStatus=1
#define	MakeOutputLow()		{PrintDebugInfo(uiDoneSteps, uiCurTimeInMicroSec, uiCurSpeedX1K);OutputPinStatus=0;}

void PrintDebugInfo(unsigned int uiDoneSteps, unsigned int uiCurTimeInMicroSec, unsigned int uiCurSpeedX1K)
{
	fprintf(fptr, "%u\t%u\t%u\n", uiDoneSteps, uiCurTimeInMicroSec, uiCurSpeedX1K);
}
int FpgaRunMotor(unsigned int uiDesPos, unsigned int uiStartSpeedX1K, unsigned int uiDeltaSpeedX1K, unsigned int uiTimeForAccInMicroSec, unsigned int uiTimeToStartDeccInMicroSec, unsigned int uiPeakSpeed)
{
	/*
		This function performs the actions which must be done by FPGA.
		So, finally this should be converted to VHDL or Verilog code for the FPGA.
	*/
	unsigned int uiDoneSteps;
	unsigned int uiCurSpeedX1K;
	unsigned int uiSpeedTimeProduct;
	unsigned int uiCurTimeInMicroSec;

	MakeOutputHigh();
	fprintf(fptr, "Inside FpgaRunMotor:\n");
	fprintf(fptr, "uiDesPos = %u\n", uiDesPos);
	fprintf(fptr, "uiStartSpeedX1K = %u\n", uiStartSpeedX1K);
	fprintf(fptr, "uiDeltaSpeedX1K = %u\n", uiDeltaSpeedX1K);
	fprintf(fptr, "uiTimeForAccInMicroSec = %u\n", uiTimeForAccInMicroSec);
	fprintf(fptr, "uiTimeToStartDeccInMicroSec = %u\n", uiTimeToStartDeccInMicroSec);
	fprintf(fptr, "step\ttime\tspeed\n");

	uiDoneSteps = 0;
	uiCurSpeedX1K = uiStartSpeedX1K;
	uiDoneSteps++;
	uiSpeedTimeProduct = uiCurSpeedX1K;
	uiCurTimeInMicroSec = 0;
	MakeOutputLow();	// first pulse output
	while (uiDoneSteps < uiDesPos)
	{
		// execute this loop after every N micro-seconds, where N = SCAN_TIME_IN_MICRO_SEC
		uiCurTimeInMicroSec += SCAN_TIME_IN_MICRO_SEC;
		uiSpeedTimeProduct += uiCurSpeedX1K;
		if (uiCurTimeInMicroSec >= uiTimeToStartDeccInMicroSec)
		{
			// it means deceleration is going on
			if (uiCurSpeedX1K > uiDeltaSpeedX1K)
			{
				uiCurSpeedX1K -= uiDeltaSpeedX1K;
			}
			else
			{
				uiSpeedTimeProduct = COUNT_FOR_COMPARISON;
				printf("\t\terror!\n");
			}
		}
		else if (uiCurTimeInMicroSec < uiTimeForAccInMicroSec)
		{
			// it means acceleration is going on
			uiCurSpeedX1K += uiDeltaSpeedX1K;
		}
		else
		{
			// it means steady speed (plateau)
			uiCurSpeedX1K = uiPeakSpeed;
		}
		if (uiSpeedTimeProduct >= COUNT_FOR_COMPARISON)
		{
			uiDoneSteps++;
			MakeOutputLow();	// next pulse output
			printf("falling edge at step %d!\n", uiDoneSteps);
			uiSpeedTimeProduct -= COUNT_FOR_COMPARISON;
		}
		else if (uiSpeedTimeProduct >= (COUNT_FOR_COMPARISON / 2))
		{
			if (!OutputPinStatus)
			{
				printf("g_nRisingEdgeCount = %d\n", g_nRisingEdgeCount);
			}
			MakeOutputHigh();
			g_nRisingEdgeCount++;
		}
	}
	MakeOutputHigh();
	return 0;
}
int RunMotor(unsigned int uiStartSpeed, unsigned int uiAvailableTimeInMicroSec, int nDesPos, unsigned int uiSpeedLimit, unsigned int uiAccLimit)
{
	/*
		This function performs the calculations which must be done by microprocessor.
		Finally, it calls another function FpgaRunMotor()
		i.e. it passes some parameters to FPGA and tells it to start motion.
	*/
	unsigned int uiAverageSpeed;
	unsigned int uiTimeForAccInMicroSec;
	unsigned int uiTimeToStartDeccInMicroSec, uiRequiredAcc;
	unsigned int uiPeakSpeed, uiDeltaSpeedX1K;

	uiRequiredAcc = 0;
	if (uiAvailableTimeInMicroSec)
	{
		uiAverageSpeed = abs(nDesPos) * 1000000UL / uiAvailableTimeInMicroSec;	// steps/sec
		if (uiAverageSpeed <= uiStartSpeed)
		{
			// no acceleration/deccelration required
			uiStartSpeed = uiAverageSpeed;
		}
		else
		{
			uiRequiredAcc = 1;
		}
	}
	else
	{
		// no acceleration/deccelration required
	}
	if (uiRequiredAcc)
	{
		if (uiAverageSpeed <= ((uiSpeedLimit + uiStartSpeed) / 2))
		{
			// tri-angular profile
			uiTimeForAccInMicroSec = uiAvailableTimeInMicroSec / 2;
			uiPeakSpeed = ((uiAverageSpeed - uiStartSpeed) * 2) + uiStartSpeed;
			uiRequiredAcc = (((uiPeakSpeed - uiStartSpeed) * 40000UL) / (uiAvailableTimeInMicroSec)) * 50;
			// please do not try to reduce above stmnt
			// becuase it results in overflow of intermediate result
		}
		else
		{
			// trapezoidal profile
			/*
				number of steps covered during acc and deacc = (Ta * (uiSpeedLimit + uiStartSpeed))
				number of steps covered during steady speed = nDesPos - (Ta * (uiSpeedLimit + uiStartSpeed))
				Ts = steady time can be computed from:
				uiSpeedLimit = (nDesPos - (Ta * (uiSpeedLimit + uiStartSpeed))) / Ts ;
				i.e.
				Ts = (nDesPos - (Ta * (uiSpeedLimit + uiStartSpeed))) / uiSpeedLimit ;
				(Ts + 2*Ta) * 1000000 = uiAvailableTimeInMicroSec ;
				Here, Ts and Ta are in seconds.
				(((nDesPos - (Ta * (uiSpeedLimit + uiStartSpeed))) / uiSpeedLimit) + (2*Ta)) * 1000000 = uiAvailableTimeInMicroSec ;
				((nDesPos - (Ta * (uiSpeedLimit + uiStartSpeed))) / uiSpeedLimit) + (2*Ta) = uiAvailableTimeInMicroSec / 1000000 ;
				multiply boths sides by uiSpeedLimit
				(nDesPos - (Ta * (uiSpeedLimit + uiStartSpeed))) + (2*Ta*uiSpeedLimit) = uiAvailableTimeInMicroSec*uiSpeedLimit / 1000000 ;
				nDesPos - Ta*uiSpeedLimit - Ta*uiStartSpeed + 2*Ta*uiSpeedLimit = uiAvailableTimeInMicroSec*uiSpeedLimit / 1000000 ;
				nDesPos - Ta*(uiStartSpeed - uiSpeedLimit) = uiAvailableTimeInMicroSec*uiSpeedLimit / 1000000 ;
				nDesPos - (uiAvailableTimeInMicroSec*uiSpeedLimit / 1000000) = Ta*(uiStartSpeed - uiSpeedLimit) ;
				Ta = (nDesPos - (uiAvailableTimeInMicroSec*uiSpeedLimit / 1000000)) / (uiStartSpeed - uiSpeedLimit) ;
				TaInUSec = (nDesPos*1000000 - uiAvailableTimeInMicroSec*uiSpeedLimit) / (uiStartSpeed - uiSpeedLimit) ;
				TaInUSec = (uiAvailableTimeInMicroSec*uiSpeedLimit - nDesPos*1000000) / (uiSpeedLimit - uiStartSpeed) ;
			*/
			uiPeakSpeed = uiSpeedLimit;
			uiTimeForAccInMicroSec = (uiAvailableTimeInMicroSec * uiSpeedLimit - nDesPos * 1000000) / (uiSpeedLimit - uiStartSpeed);
			uiRequiredAcc = (((uiSpeedLimit - uiStartSpeed) * 40000U) / uiTimeForAccInMicroSec) * 25;
			// please do not try to reduce above stmnt
			// becuase it results in overflow of intermediate result
			if (uiRequiredAcc > uiAccLimit)
			{
				// handle this error condition
				// TBD
			}
		}
		uiTimeToStartDeccInMicroSec = uiAvailableTimeInMicroSec - uiTimeForAccInMicroSec;
		uiDeltaSpeedX1K = SCAN_TIME_IN_MICRO_SEC * uiRequiredAcc / 1000;
		if (!uiDeltaSpeedX1K)
		{
			uiDeltaSpeedX1K = 1;
		}
		// do something to set/clear direction pin here
		FpgaRunMotor(abs(nDesPos), uiStartSpeed * 1000, uiDeltaSpeedX1K, uiTimeForAccInMicroSec, uiTimeToStartDeccInMicroSec, uiPeakSpeed * 1000);
	}
	else
	{
		// run steady at uiStartSpeed for N steps and then stop
		// do something to set/clear direction pin here
		FpgaRunMotor(abs(nDesPos), uiStartSpeed * 1000, 0, 0, uiAvailableTimeInMicroSec, uiStartSpeed * 1000);
	}
	fprintf(fptr, "Required acceleration = %u Steps/sec/sec\n", uiRequiredAcc);
	return uiRequiredAcc;
}

int main2(int argc, char* argv[])
{
	unsigned int uiStartSpeed = START_SPEED;
	unsigned int uiAvailableTimeInMiliSec = AVAILABLE_TIME_IN_MILI_SEC;
	int nDesPos = DES_POS;
	unsigned int uiSpeedLimit = SPEED_LIMIT;
	unsigned int uiAccLimit = ACC_LIMIT;
	int k;

	if ((argc < 2) || (argc > 7))
	{
		printf("Syntax: step outputfilename [AvailableTime] [DesPos] [StrtSpeed] [SpeedLimit] [AccLimit]\n");
		return 1;
	}
	fptr = fopen(argv[1], "wt");
	if (fptr == NULL)
	{
		printf("Could not open file %s\n", argv[1]);
		return 2;
	}
	for (k = 2; k < argc; k++)
	{
		switch (k)
		{
		case	2:
			uiAvailableTimeInMiliSec = atoi(argv[k]);
			break;
		case	3:
			nDesPos = atoi(argv[k]);
			break;
		case	4:
			uiStartSpeed = atoi(argv[k]);
			break;
		case	5:
			uiSpeedLimit = atoi(argv[k]);
			break;
		case	6:
			uiAccLimit = atoi(argv[k]);
			break;
		}
	}
	fprintf(fptr, "\n\t\tProfile Generation Algorithm Test Program\n");

	fprintf(fptr, "\nSpeed Limit in Steps/Sec is:%u", uiSpeedLimit);
	fprintf(fptr, "\nAcceleration Limit in Steps/Sec/Sec is:%u", uiAccLimit);
	fprintf(fptr, "\nStart Speed in Steps/Sec is:%u", uiStartSpeed);
	fprintf(fptr, "\nDesired Position (Number of Steps) is:%u", nDesPos);
	fprintf(fptr, "\nAvailable time in mili seconds is:%u\n\n", uiAvailableTimeInMiliSec);

	RunMotor(uiStartSpeed, (uiAvailableTimeInMiliSec * 1000), nDesPos, uiSpeedLimit, uiAccLimit);

	fclose(fptr);
	return 0;
}