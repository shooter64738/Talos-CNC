#ifndef __C_MOTION_CORE_KINEMATICS_H
#define __C_MOTION_CORE_KINEMATICS_H
#include <stdint.h>

#include "../../physical_machine_parameters.h"
#include "../../_bit_flag_control.h"

/*

All control flags for motion are in here. If its a flag that has something to do with the motion
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all motion output from.

*/

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Kin
			{

				class Numbers
				{
				public:

					struct s_axis_config
					{
						uint32_t steps_per_unit = 200;
						float acceleration;
						uint32_t max_speed = 5;

					};

					struct s_axis_integrator
					{
						s_axis_config axis_settings;

						uint32_t stp_dst;
						float acceleration;
						float accel_time;
						float accel_distance;
						float traverse_distance;
						float traverse_position;
						float steps_at_time = 0.0;
						float speed_at_time = 0.0;
						float accel_speed = 0.0;
						float accel_position = 0.0;

						uint8_t ramp_state = 0;

						void setup(float major_unit_distance, float axis_unit_distance, float axis_unit_per_minute_rate
							, float unit_acceleration_per_s)
						{
							float time_scaler = 0.001;
							axis_settings.acceleration = unit_acceleration_per_s;
							//scale this axis unit rate so that it will take the same TIME as the major axis
							float major_rate_adjust = axis_unit_distance / major_unit_distance;
							//determine how many UNITS per second we are moving
							float units_per_second = (axis_unit_per_minute_rate * major_rate_adjust) / 60;
							//convert the unit distance to a step distance
							stp_dst = axis_unit_distance;// *axis_settings.steps_per_unit;
							//convert acceleration from units per second to steps per second
							acceleration = axis_settings.acceleration;// unit_acceleration_per_s;// *axis_settings.steps_per_unit;
							//how long can we accelerate
							accel_time = axis_settings.max_speed / acceleration;
							//how far can we accelerate
							accel_distance = (axis_settings.max_speed *.5) * accel_time;
							if (accel_distance > axis_unit_distance *.5)
							{
								//speed profile is triangle
								accel_distance = axis_unit_distance * .5;
								traverse_distance = 0;
							}
							else
							{
								traverse_distance = axis_unit_distance - accel_distance * 2;
								//speed profile is trapezoidal
							}
							/*acceleration *= axis_settings.steps_per_unit;
							acceleration *= time_scaler;
							accel_distance *= axis_settings.steps_per_unit;*/
							ramp_state = 1; //accel

						}
						uint32_t step_inc = 0;
						uint32_t check_pos(uint32_t time)
						{
							
							speed_at_time = (acceleration * time) + accel_speed;
							steps_at_time = accel_position+ ((acceleration * time*(time + 1) / 2)+ accel_speed);

							
							if (steps_at_time > accel_distance+traverse_distance)
							{
								accel_speed = speed_at_time+time;
								accel_position = steps_at_time;
								acceleration = -1;

							}
							else if (steps_at_time > accel_distance)
							{
								accel_speed = speed_at_time;
								accel_position = steps_at_time;
								acceleration = 0;
							}
							/*speed_at_time = (acceleration * time);
							steps_at_time = acceleration * time*(time + 1) / 2;
							*/

							/*
							Seems as tho simple addition works just as well as complex formulas and its faster.
							this could be called every millisecond to update the pwm counter values and keep the
							output in line with the appropriate speeds
							*/
							//speed_at_time += acceleration;

							return 0;
						}

					};

					struct s_integrator_control
					{
						float accel_rate = 1;
						s_axis_integrator axis[6];

					};



					static s_integrator_control int_ctrl;

					static void test3(uint32_t clocks);
					static void init();

					static void test(uint32_t delta_time);
					static void test2(uint32_t delta_time);
				};
			};
		};
	};
};
#endif