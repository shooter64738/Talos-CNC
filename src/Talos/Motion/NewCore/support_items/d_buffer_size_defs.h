#ifndef __C_MOTION_CORE_BUFFER_SIZE_DEFINES_H
#define __C_MOTION_CORE_BUFFER_SIZE_DEFINES_H

#define NGC_BUFFER_SIZE 15 //<--how many block can come into the motion controller
#define MOTION_BUFFER_SIZE 15//<--how many incoming blocks can we convert to motion
#define BRESENHAM_BUFFER_SIZE 15 //<--number of bresenham items, shoudl be TIMER_BUFFER_SIZE-1
#define TIMER_BUFFER_SIZE 5//<--Number of timer segment items we can hold. As large as permitted.

#endif