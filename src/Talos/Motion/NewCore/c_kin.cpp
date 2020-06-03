/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

/*

All control flags for motion are in here. If its a flag that has something to do with the motion
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all motion output from.

*/


#include "c_kin.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Kin
			{

				Numbers::s_integrator_control Numbers::int_ctrl{ };
				

				float programmed_unit_minute_rate = 1; //<--how fast are we supposed to go
				float major_unit_distance = 1; //<-- one of the axis has the longest distance, (or all have same distance) what distance is greatest?
				float axis1_unit_distance = 50; //<--axis 1 distance
				float axis2_unit_distance = 1; //<--axis 2 distance
				float axis3_unit_distance = 1; //<--axis 3 distance
				
				void Numbers::init()
				{
					Numbers::int_ctrl.axis[0].setup(major_unit_distance, axis1_unit_distance, programmed_unit_minute_rate, int_ctrl.accel_rate);
					Numbers::int_ctrl.axis[1].setup(major_unit_distance, axis1_unit_distance, programmed_unit_minute_rate, int_ctrl.accel_rate);
					Numbers::int_ctrl.axis[2].setup(major_unit_distance, axis1_unit_distance, programmed_unit_minute_rate, int_ctrl.accel_rate);
				}

				void Numbers::test3(uint32_t clocks)
				{
					
					
					//assume axis1 is the major axis, is runs the fastest and has the shortest ms step time
					//Numbers::int_ctrl.axis[0].check_pos(clocks);
					Numbers::int_ctrl.axis[0].check_pos(clocks);
					
					
					//assume axis2 is a subortinate axis, it runs slower than axis1 but faster than axis 3.
					//Numbers::int_ctrl.axis[1].check_pos(clocks);
					//Numbers::int_ctrl.axis[1].get_cur_spd(clocks);
					//Numbers::int_ctrl.axis[1].get_cur_pos(clocks);
					//
					////assume axis2 is a subortinate axis, it runs slower than axis2 and is the slowest
					//Numbers::int_ctrl.axis[2].check_pos(clocks);
					//Numbers::int_ctrl.axis[2].get_cur_spd(clocks);
					//Numbers::int_ctrl.axis[2].get_cur_pos(clocks);
					
					


				}
				/*
				Hardware:
				Main cpu creates a PWM clock signal.
				Each axis controller is connected to main clock signal
				Clock signal regulates the speed of the 'master' axis
				Each axis may or may not move at different speeds
				The 'master' axis will move the fastest
				Each axis control will issue its own PWM driver signal to a stepper/servo control

				Software:
				When a motion block is loaded each axis control will recieve parameters for its axis
				After all axis controls are updated motion can start
				Each axis controller will read the master clock time and determine the output
					frequency needed to move at a proportional rate
				Adjusting the master clock speed will cause ALL axis controllers to speed up or slow down
				Each axis controller will adjust its own output to determine if its rate is correct to move
					at the programed rate

				*/


				//inegrator
				/* basics
				200 steps per turn
				axis 1 turn per mm
				micro step is 16
				step per mm is 200x16x1 = 3200
				speed is 100mm/s
				320,000 steps/s
				step seperation is 1/320,000 = 3.125us
				*/

				/*
				speed = speed + accleration
				position = position + speed
				accel by +1 5 clocks (accelerate)
				accel by +0 for 5 clocks (maintian speed)
				accel by -1 for 5 clocks (decelerate)

				after 15 clocks we arrive at position +50
				*/

				float t_pos = 0; //<--pos in X seconds
				float t_spd = 0; //<--speed in X seconds
				float mm_accel = 150; //mm/s //<--mm/s/s acceleration
				uint32_t stp_accel = mm_accel * 200; //step/s/s //<--step/s/s acceleration
				float stp_accel_ms = stp_accel / 1000; //step/s/s //<--step/s/s acceleration
				float stp_accel_mc = stp_accel_ms / 1000; //step/s/s //<--step/s/s acceleration
				void Numbers::test(uint32_t delta_time)
				{
					uint32_t steps_per_milli = stp_accel / 1000;
					uint32_t steps_per_micro = stp_accel / 1000000;

					//mm/s
					t_spd = (delta_time)* mm_accel;
					t_pos = (mm_accel * (delta_time * (delta_time + 1)) / 2) + delta_time * t_spd;

					//stp/ms
					t_spd = (delta_time)* stp_accel_ms;
					t_pos = (stp_accel_ms * (delta_time * (delta_time + 1)) / 2) + delta_time * t_spd;

					//stp/mc
					t_spd = (delta_time)* stp_accel_mc;
					t_pos = (stp_accel_mc * (delta_time * (delta_time + 1)) / 2) + delta_time * t_spd;
					if (t_pos > 1)
					{
						int x = 0;
					}


					/*generate
						for (i = 0; i < nbaxes; i = i + 1)
							begin:motion
							always @(posedge clk) axes_S[i] <= axes_S[i] + axes_A[i];
					always @(posedge clk) axes_P[i] <= axes_P[i] + axes_S[i];
					end
						endgenerate

						position = ((a*(t^2))/2)+t*s
						*/

						/*
						all values in steps per second
						begin at start speed
						accel up to speed limit
						decel back to start speed
						triangle (avg speed) = (speedlimit+startspeed)/2
						distance in time = availabletime * average speed
						if distance >=desired_position then triangle
						required avg speed = desired position/available time
						required max speed = (required avg speed*2)-start speed
						make sure required max speed does nto exceed speed limits

						assuming triangle profile
						required acceleration = (required max speed-start speed)*2/available time
						make sure required acceleration does nto exceed speed limits

						T1= 1/start speed
						speed at T1 = start speed+ required acceleration *T1
						T2= 1/(start speed+(required acceleartion *T1)

						//new way
						check time
						multiply by speed
						if current position< calculated position, move ahead X steps.

						Assume .2 steps/sec
						assume T=0
						speed-time product = time * .2
						after 5 seconds 5*.2, speed-time = 1 and we can step
						after 10 second 10*.2 speed-time = 2 and we can step.
						using delta time, every time speed gets to 5, we can step.
						every time we step add 1 to current position
						we can leave out multiplcation and use speed-time += speed

						To do the calculation in milliseconds
						speed-time += speed/1000

						To do the calculation in microseconds
						speed-time += speed/1000000
						*/
					void test2(uint32_t delta_time);
					{
						static float req_speed = 5;
						static float req_speed_micro = req_speed / 1000000; //mm/s
						static float speed_time = 0;
						static uint32_t step_pos = 0;
						t_spd = (delta_time)* mm_accel;
						speed_time += req_speed_micro;

						if (speed_time > 1)
						{
							step_pos++;
						}



					}
				}
			}
		}
	}
}