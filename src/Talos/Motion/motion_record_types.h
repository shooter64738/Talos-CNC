
//record types for eventing, responses, system events etc, are not the same between
//all projects. Events, errors, responses, etc for the coordinator are not alsway
//appropriate  for the motion, spindle, peripheral controls. Moving these specific
//types to ehre as an include will create a seperation between them and make the 
//code easier to manage and debug.


#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__)
#define MSVC
#endif

#ifndef MOTION_HARDWARE_DEF_H_
#define MOTION_HARDWARE_DEF_H_

#ifdef __AVR_ATmega2560__
#endif

#ifdef __SAM3X8E__
#endif

#ifdef MSVC
#endif


#endif 