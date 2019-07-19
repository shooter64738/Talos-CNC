/*
 * physical_machine_parameters.h
 *
 * Created: 7/12/2019 7:25:04 PM
 *  Author: Family
 */ 


#ifndef PHYSICAL_MACHINE_PARAMETERS_H_
#define PHYSICAL_MACHINE_PARAMETERS_H_

#define MACHINE_AXIS_COUNT 6
#define MACHINE_X_AXIS 0
#define MACHINE_Y_AXIS 1
#define MACHINE_Z_AXIS 2
#define MACHINE_A_AXIS 3
#define MACHINE_B_AXIS 4
#define MACHINE_C_AXIS 5
#define MACHINE_U_AXIS 6
#define MACHINE_V_AXIS 7

/*
Changing the machine type define will determine how the code is compiled to
interpret gcode for a specific type of machine. Mill gcode cannot be interpretted
the same way as lathe gcode. Plasma machine gcode also has its own flavor, etc..
*/
//Uncomment out the specific type of machine you need compilation for. 
//#define MACHINE_TYPE_MILL
#define MACHINE_TYPE_LATHE

#endif /* PHYSICAL_MACHINE_PARAMETERS_H_ */