/*
* c_torch_height.cpp
*
* Created: 1/26/2019 11:28:06 AM
* Author: jeff_d
*/


#include "c_torch_height.h"
#include "..\Common\Hardware_Abstraction_Layer\c_hal.h"
uint8_t Plasma_Control::c_torch_height::move_head(Plasma_Control::e_torch_positions Target)
{
	//drive the torch height stepper
	c_hal::driver.PNTR_SET_PRESCALER(8);
	c_hal::driver.PNTR_SET_TIMER_RATE(24000);
	c_hal::driver.PNTR_DRIVE(); //<--activate the driver.

	//Stepper is now in motion. We need to read the input for the limit switch AND contact on the work piece
	uint8_t result = 0;
	while (1)
	{
		result = c_hal::io.PNTR_GET_DIGITAL(DIGITAL_INPUT_PORT);
		if (result &(1 << LOWER_LIMIT_SWITCH_INPUT_PIN)
			|| result & (1 << MATERIAL_SWITCH_INPUT_PIN))
		{
			c_hal::driver.PNTR_DISABLE();//<--turn off the stepper;
			//determine which condition we detected. If we hit the limit switch before the material, then its an error
			if (result &(1 << LOWER_LIMIT_SWITCH_INPUT_PIN))
			{
				//Move the torch back to the top
				c_hal::driver.PNTR_DRIVE(); //<--activate the driver.
				while (1)
				{
					result = c_hal::io.PNTR_GET_DIGITAL(DIGITAL_INPUT_PORT);
					if (result &(1 << UPPER_LIMIT_SWITCH_INPUT_PIN))
					{
						c_hal::driver.PNTR_DISABLE();//<--turn off the stepper;
						return Plasma_Control::Errors::MATERIAL_OUT_OF_RANGE;
					}
				}
			}
		}
		return Plasma_Control::Errors::OK;
	}
}
//// default constructor
//c_torch_height::c_torch_height()
//{
//} //c_torch_height
//
//// default destructor
//c_torch_height::~c_torch_height()
//{
//} //~c_torch_height


/*
// Libraries ===========================================
#include <AccelStepper.h>

// Pin Assignments =====================================
const byte arcVoltPin = 1;                              //analog input

const byte xStepInPin = 2;                              //digital input  0 and 1 are reserved for TX/RX
const byte yStepInPin = 3;                              //digital input  2 and 3 are the only valid interrupt pins
const byte spinEnInPin = 5;                             //digital input

const byte zStepControl = 6;                            //digital output  High Level Trigger Relay
const byte zDirControl = 7;                             //digital output  Low Level Trigger Relay
const byte zStepOutPin = 10;                            //digital output
const byte zDirOutPin = 13;                             //digital output

// Parameters to tune ==================================
float vHyst = 1;                                        //Hysteresis for OK-Band during THC closed-loop operation (undivided arc voltage, V).  If the measured voltage is within the target, +/- vHyst, tortch height is not changed
float zBacklash = 1.81;                                 //mm, backlash in z axis
float THCsteps = 0.5;                                   //mm of adjustment on every THC iteration in closed feedback loop control
float loopFreq = 5;                                     //Hz, frequency of Z adjustment in closed-loop
float zMaxSpd = 10;                                     //mm/s
float zMaxAccel = 1000;                                 //mm/s^2
float ArcVoltDivisor = 100.6;                           //Voltage divider, set at 50:1 (times a 2.012 op amp gain)
float xstepspmm = 20.32;                                //X steps per mm
float ystepspmm = 20.32;                                //Y steps per mm
float zstepspmm = 102.00;                               //Z steps per mm
float closedLoopSpeed = 400;                            //Z steps per sec
float closedLoopAccel = 50000;                          //Z steps per sec^2
int sampleSize = 80;                                    //Number of samples for running average.  Max of (1000/loopFreq)

// Initialize variables ================================
String tTarg;
float startx;
float starty;
float startt;
float endt;
float freqx;
float freqy;
float freqPythag;
volatile unsigned int xsteps;
volatile unsigned int ysteps;
float arcVsum;
float arcVavg;
float vTarg;
float frTarg;
boolean validInput = false;
boolean lastMoveUp;
unsigned int charCount;
float i;
unsigned int floatLowCount;
unsigned int floatHighCount;
float arcV2Counts = (1024/(5*ArcVoltDivisor));
unsigned int SpinEnInCount;

// Define a stepper and the pins it will use ===========
AccelStepper stepper(AccelStepper::DRIVER, zStepOutPin,
zDirOutPin);


void setup() {
	// Set input pins ====================================
	pinMode(xStepInPin, INPUT);
	pinMode(yStepInPin, INPUT);
	pinMode(spinEnInPin, INPUT);
	pinMode(zStepControl, OUTPUT);
	pinMode(zDirControl, OUTPUT);
	pinMode(zStepOutPin, OUTPUT);
	pinMode(zDirOutPin, OUTPUT);
	
	// AccelStepper Parameters ===========================
	stepper.setPinsInverted(true, false, false);          //Direction, Step, En, TB6600 Driver
	
	// Interrupt Parameters for measuring X/Y speed ======
	attachInterrupt(digitalPinToInterrupt(xStepInPin),
	xcount, RISING);
	attachInterrupt(digitalPinToInterrupt(yStepInPin),
	ycount, RISING);

	// Get target thickness from serial monitor ==========
	Serial.begin(9600);                                   //Start the serial buffer
	while (!validInput) {                                 //Until a valid input has been entered
		tTarg = SerialInput("material thickness");          //Target thickness
		
		// Correlate material thickness ====================//Hypertherm Powermax 45XP
		if      (tTarg == "16ga") {
			frTarg = 249*25.4/60;                             //inches/min to mm/s
			vTarg = 128;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("16 ga identified");
		}
		else if (tTarg == "14ga") {
			frTarg = 225*25.4/60;                             //inches/min to mm/s
			vTarg = 128;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("14 ga identified");
		}
		else if (tTarg == "10ga") {
			frTarg = 129*25.4/60;                             //inches/min to mm/s
			vTarg = 128;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("10 ga identified");
		}
		else if (tTarg == "3/16") {
			frTarg = 85*25.4/60;                              //inches/min to mm/s
			vTarg = 129;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("3/16 identified");
		}
		else if (tTarg == "1/4") {
			frTarg = 48*25.4/60;                              //inches/min to mm/s
			vTarg = 130;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("1/4 identified");
		}
		else if (tTarg == "3/8") {
			frTarg = 33*25.4/60;                              //inches/min to mm/s
			vTarg = 136;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("3/8 identified");
		}
		else if (tTarg == "1/2") {
			frTarg = 18*25.4/60;                              //inches/min to mm/s
			vTarg = 141;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("1/2 identified");
		}
		else if (tTarg == "0.1") {
			frTarg = 184*25.4/60;                             //inches/min to mm/s
			vTarg = 128;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("0.100\" identified");
		}
		else if (tTarg == "1/8") {
			frTarg = 144*25.4/60;                             //inches/min to mm/s
			vTarg = 128;                                      //Volts
			validInput = true;                                //boolean
			Serial.println("1/8\" identified");
		}
		else {
			Serial.println("Invalid Response");
		}
	}
}

// Interrupt Functions for XY steps ====================
void xcount() {
	xsteps++;
}

void ycount() {
	ysteps++;
}

void moveWithBlock() {
	while (stepper.run()) {                               //Could this be while (stepper.run()) ?
		stepper.run();
	}
}

// Serial Input Function ===============================
String SerialInput(String InputParam) {
	Serial.print("Enter the ");
	Serial.print(InputParam);
	Serial.println(". [16ga, 14ga, 10ga, 1/8, 3/16, 1/4, 3/8, 1/2, 0.1]");
	String Input;
	while(Serial.available() == 0) { }                    // There really is nothing between the {} braces, delays until something is entered.
	if (Serial.available() > 0) {                         //If there is anything in the buffer
		Input = Serial.readString();                        //parse the input from the buffer
	}
	return Input;
}

void loop() {
	// Set Z-stepper Speed/Accel
	stepper.setMaxSpeed(zMaxSpd*zstepspmm);               //mm/s (scaled to steps)
	stepper.setAcceleration(zMaxAccel*zstepspmm);         //mm/s^2 (scaled to steps)
	
	//Give Z control to grbl through relay ===============
	Serial.println("Pass-through Z from grbl");
	digitalWrite(zStepControl, LOW);
	digitalWrite(zDirControl, HIGH);
	delay(10);
	SpinEnInCount = 0;
	while (SpinEnInCount < 3) {
		SpinEnInCount = SpinEnInCount+digitalRead(spinEnInPin); //If there is no spinEn command, wait
		delay(30);
	}

	// When SpinEn goes HIGH, take Control of the Z axis
	digitalWrite(zStepControl, HIGH);                     //Closes high
	digitalWrite(zDirControl, LOW);                       //Closes low (different relay type)

	// Closed loop THC ===================================
	Serial.print("Target Voltage: ");
	Serial.print(vTarg-vHyst);
	Serial.print(" to ");
	Serial.print(vTarg+vHyst);
	Serial.println(" volts");
	
	while (digitalRead(spinEnInPin) == HIGH){             //Only do when torch is on
		// Get starting X and Y positions ==================
		startx = xsteps;
		starty = ysteps;
		startt = millis();
		
		// Get the average arc voltage =====================
		i = 0;
		arcVsum = 0;
		while ((millis()-startt) <= ((1/loopFreq)*1000)){   //Calculate avg voltage over delay period
			//delay(((1/loopFreq)/sampleSize)*1000);            //(1/loopFreq) = Period.  Period/sampleSize => sampleSize samples during delay.  1/sampleSize * 1000 for ms
			arcVsum = arcVsum+analogRead(arcVoltPin);
			i++;
		}
		arcVavg = (arcVsum/i)/arcV2Counts;                  //Average arc voltage (sum/numel) * (D to A) * ArcVoltDivisor
		Serial.print(arcVavg);
		Serial.print(" ");
		
		// Calculate X and Y speeds ========================
		endt = millis();
		freqx = ((xsteps-startx)/xstepspmm)/
		((endt-startt)/1000);                             //dx/dt, mm/s
		freqy = ((ysteps-starty)/ystepspmm)/
		((endt-startt)/1000);                             //dy/dt, mm/s
		freqPythag = sqrt(sq(freqx)+sq(freqy));             //dv/dt, mm/s of torch head in XY plane
		
		// Control Torch Height ============================
		stepper.setMaxSpeed(closedLoopSpeed);               //Set stepper speed for closed-loop THC control
		stepper.setAcceleration(closedLoopAccel);           //Set stepper accel for closed-loop THC control
		if (freqPythag >= (0.8*frTarg)) {                   //If the speed is within 80% of the target
			// Too High, Move Down
			if (arcVavg > (vTarg+vHyst)) {                    //If the arc voltage is greater than the target plus the hysteresis window
				if (lastMoveUp) {                               //If the last move was up
					stepper.move(-((THCsteps+zBacklash)*zstepspmm));//Move target down with backlash compensation
					moveWithBlock();
				}
				else {                                          //If the last move was down
					stepper.move(-(THCsteps*zstepspmm));          //Move down without adding backlash
					moveWithBlock();
				}
				Serial.println("v");
				charCount++;
				lastMoveUp = false;                             //Indicate that the last move was down
			}
			// Too Low, Move Up
			else if (arcVavg < (vTarg-vHyst)) {               //If the arc voltage is less than the target minus the hysteresis window
				if (lastMoveUp) {                               //If the last move was up
					stepper.move(THCsteps*zstepspmm);             //Move up more without accounting for backlash
					moveWithBlock();
				}
				else {                                          //If the last move was down
					stepper.move((THCsteps+zBacklash)*zstepspmm); //Move up with backlash compensation
					moveWithBlock();
				}
				Serial.println("^");
				charCount++;
				lastMoveUp = false;                                 //Indicate that the last move was up
			}
			// OK, No Move
			else {
				Serial.println("-");
				charCount++;
			}
		}
		else {
			Serial.println("S");
			charCount++;
		}
		
		// Carriage return Up/Down feedback
		if (charCount >= 50) {
			Serial.println("");
			charCount = 0;
		}
	}
	Serial.println("");
}*/