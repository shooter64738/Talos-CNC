/* 
* c_print.h
*
* Created: 3/6/2019 1:15:57 PM
* Author: jeff_d
*/

/*
  print.h - Functions for formatting output strings
  Part of Grbl

  Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __C_PRINT_H__
#define __C_PRINT_H__

#include <stdint.h>
class c_print
{
//variables
public:
protected:
private:

//functions
public:


static void print_string(const char *s);

static void print_string_P(const char *s);

static void print_int32(long n);

static void print_uint8_base10(uint8_t n);

static void print_uint32_base10(uint32_t n);

// Prints an uint8 variable in base 2 with desired number of desired digits.
static void print_uint8_base2_ndigit(uint8_t n, uint8_t digits);

static void print_float(float n, uint8_t decimal_places);

// Floating value printing handlers for special variables types used in Grbl.
//  - CoordValue: Handles all position or coordinate values in inches or mm reporting.
//  - RateValue: Handles feed rate and current velocity in inches or mm reporting.
static void print_float_coord_value(float n);
static void print_float_rate_value(float n);

// Debug tool to print free memory in bytes at the called point. Not used otherwise.
static void print_free_memory();
protected:
private:

}; //c_print

#endif //__C_PRINT_H__
