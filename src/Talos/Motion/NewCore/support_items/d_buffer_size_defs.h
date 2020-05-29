#ifndef __C_MOTION_CORE_BUFFER_SIZE_DEFINES_H
#define __C_MOTION_CORE_BUFFER_SIZE_DEFINES_H

#define NGC_BUFFER_SIZE 100 //<--how many block can come into the motion controller
#define MOTION_BUFFER_SIZE 100//<--how many incoming blocks can we convert to motion
#define SPINDLE_BUFFER_SIZE 5//<--Number of spindle control records. Probably doesnt need to be a high value
#define BRESENHAM_BUFFER_SIZE 100 //<--number of bresenham items, shoudl be MOTION_BUFFER_SIZE-1
#define TIMER_BUFFER_SIZE 400//<--Number of timer segment items we can hold. As large as permitted.

#endif