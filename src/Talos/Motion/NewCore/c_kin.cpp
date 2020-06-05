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

#include <math.h>
#define PI 3.14142
#define pwm_pin 2
#define F_BUS 50
#define ftm 3
#define alt 4
#define FTM_CHANCFG(chan) FTM_CHANCFG2(chan)
#define FTM_CHANCFG2(chan) FTM ## chan ## _C0V
#define FTM_PINCFG(pin) FTM_PINCFG2(pin)
#define FTM_PINCFG2(pin) CORE_PIN ## pin ## _CONFIG

				// input values:
				double delta_t_ms = 0.2;                        // ramp stage duration in ms
				double f_min = 3000.0;                          // start frequency in steps/s: if too slow for delta_t f_min will be increased in init()
				double a_f_max_ms = 500.0;                      // max possible frequency increment per ms in steps/ms
				double steps_R = 200.0;                         // steps per motor revolution
				double microsteps = 16.0;                       // microstepping in microsteps per step

				// timing belt or rack & pinion:
				const double diameter_mm = 40.0;                // diameter of pinion in mm
				const double ratio = 1.0;                       // gear ratio (> 1 = slower speed)

				// or lead screw:
				const double threadpitch_mm = 8.0;              // thread pitch in mm: set to zero if pinion driven

				// desired:
				double s_wanted_cm = 16.0;                      // desired travel in cm
				double v_wanted_cms = 25.0;                     // desired speed in cm/s
				double s_c_min = 0.001;                         // minimum travel with constant speed in m

				double delta_t;                                 // ramp stage duration (derived from delta_t_ms) in s
				double delta_t_us;                              // ramp stage duration (derived from delta_t_ms) in µs
				double delta_f;                                 // frequency increment per ramp stage
				double a_f_max;                                 // max possible frequency increment per ms (derived from a_f_max_ms) in steps/ms
				double s_wanted;                                // desired travel (derived from s_wanted_cm) in m
				double v_wanted;                                // desired speed (derived from v_wanted_cms) in m/s
				double a_f;                                     // actual acceleration in steps/s^2
				double t_a;                                     // duration of acceleration in s
				double t_c;                                     // duration of travel with constant speed v_wanted in s
				double t_c_ms;                                  // duration of travel with constant speed v_wanted in ms
				double t_c_us;                                  // duration of travel with constant speed v_wanted in µs
				double N;                                       // total number of ramp stages
				double n;                                       // actual ramp stage
				double u;                                       // conversion factor frequency to speed v/f in m/steps
				double f;                                       // actual frequency in steps/s
				double f_wanted;                                // desired frequency in steps/s
				double t_n_us;                                  // time stamp of ramp stage in µs

				double t_total;                                 // total time = 2 * t_a + t_c
				double s_c;                                     // travel with constant speed in m
				double a;                                       // acceleration in m/s^2
				double v;                                       // achievable max speed in m/s
				double s_a;                                     // travel of ramp in m

				uint32_t mck = F_BUS;                           // PWM source frequency
				const uint8_t duty_percent = 50;                // 50% duty cycle
				const uint8_t resolution_pwm = 12;              // PWM resolution
				uint8_t prescaler;                              // smallest possible divider will be calced
				uint16_t duty;                                  // duty cicle value

			

				void Numbers::setup() {
					/*pinMode(pwm_pin, OUTPUT);
					analogWriteResolution(resolution_pwm);*/
					init();
				}

				void Numbers::loop() {
					start();
					//delay(1000);
				}

				double Numbers::calc_freq(double freq) {
					uint16_t period = calc_period(freq);
					return (double)(mck >> prescaler) / (double)(period + 1);
				}

				uint32_t Numbers::calc_period(double freq) {
					for (prescaler = 0; prescaler < 7; prescaler++) {
						double freq_min = (double)(mck >> prescaler) / 65536.0;
						if (freq >= freq_min) break;
					}
					return (double)(mck >> prescaler) / ((double)freq) - 0.5;
				}

				double Numbers::simul_ramp(double f_min, double a_f, double delta_t, uint32_t N) {
					// simulation of ramp in order to obtain accurate travel
					double delta_f = a_f * delta_t;
					double f = f_min;
					double s_f = 0.0;                                                         // travel / u, where u = v / f
					uint32_t n = 0;
					while (n < N) {
						s_f += floor(delta_t * calc_freq(f));
						f += delta_f;
						n += 1;
					}
					return (double)u * s_f;
				}

				void Numbers::init() {
					delta_t = delta_t_ms / 1000.0;                                            // time increment in s
					a_f_max = a_f_max_ms * 1000.0;                                            // frequency increment in steps/s^2
					s_wanted = s_wanted_cm / 100.0;                                           // desired travel in m
					v_wanted = v_wanted_cms / 100.0;                                          // desired speed in m/s

					if (threadpitch_mm == 0) {
						u = (PI * (diameter_mm / 1000.0) / ratio) / (steps_R * microsteps);     // in m/steps
					}
					else {
						u = threadpitch_mm / 1000.0 / (steps_R * microsteps);                   // in m/steps
					}

					// calculations with the presumption that speed is the limiting factor
					f_wanted = v_wanted / u;                                                  // in steps/s

					// corrections for frequencys
					f_wanted = calc_freq(f_wanted);
					if (f_min < ceil(1000.0 / delta_t_ms)) f_min = ceil(1000.0 / delta_t_ms); // update f_min if too slow for delta_t
					f_min = calc_freq(f_min);

					a_f = a_f_max;                                                            // in steps/s^2
					t_a = (f_wanted - f_min) / a_f;                                           // in s
					N = floor(t_a / delta_t) + 1;

					// correction of a_f in order to obtain equal ramp stages of f for N ramp stages of delta_t
					a_f = (f_wanted - f_min) / (N * delta_t);

					// re-calculation with new a_f
					t_a = N * delta_t;

					// predict minimum needed travel to reach v_wanted
					s_a = simul_ramp(f_min, a_f, delta_t, N);

					// in case s_wanted is the limiting factor
					if (2 * s_a + s_c_min > s_wanted) {

						// calculations with the presumption that travel is the limiting factor
						// s_wanted/2 is limiting the duration of the ramp
						a_f = a_f_max;
						s_a = s_wanted / 2 - s_c_min;
						t_a = sqrt((2 * s_a / (u * a_f)) + ((f_min / a_f) * (f_min / a_f))) - (f_min / a_f);

						// recalculate max possible speed
						f_wanted = calc_freq(a_f * t_a);

						// re-calculate number of ramp stages
						N = floor(t_a / delta_t) + 1;

						// correction of a_f in order to obtain equal ramp stages of f for N ramp stages of delta_t
						a_f = (f_wanted - f_min) / (N * delta_t);

						// re-calculation with new a_f
						t_a = N * delta_t;

						// re-predict travel
						s_a = simul_ramp(f_min, a_f, delta_t, N);
					}

					s_c = s_wanted - 2 * s_a;                                                              // travel with constant speed in m
					v = u * f_wanted;                                                                      // reached speed (v <= v_wanted) in m/s
					a = u * a_f;                                                                           // acceleration in m/s^2
					t_c = s_c / v;                                                                         // duration of travel with constant speed in s
					t_total = 2 * t_a + t_c;                                                               // total duration in s

					// preparations:
					delta_f = a_f * delta_t;
					delta_t_us = delta_t_ms * 1000.0;
					t_c_us = t_c * 1000000.0;
					duty = ((uint32_t)((duty_percent << resolution_pwm) * 0.01) * (uint32_t)(calc_period(f_wanted + 1))) >> resolution_pwm;
				}

				void Numbers::start() {
					// ramp-up
					double t_pulse_us;
					f = f_min;
					n = 0;
					t_n_us = 0.0;
					//if (FTM_CHANCFG(ftm) == 0) (t_n_us += round(1000000 / f_min));
					t_n_us += micros();
					while (n < N)
					{
						//analogWriteFrequency(pwm_pin, f);
						if (n == 0)//<--first step sets up HW pwm out at start freq
						{
							//FTM_CHANCFG(ftm) = duty;
							//FTM_PINCFG(pwm_pin) = PORT_PCR_MUX(alt) | PORT_PCR_DSE | PORT_PCR_SRE;
						}
						n += 1;
						// calculate next time stamp:
						t_pulse_us = 1000000.0 / calc_freq(f);
						t_n_us += round(floor(delta_t_us / t_pulse_us) * t_pulse_us);
						f += delta_f;
						// needed waiting time = next time stamp minus last time stamp:
						while (micros() <= t_n_us);
					}

					// travel with constant speed
					//analogWriteFrequency(pwm_pin, f_wanted);
					n = 0;
					t_pulse_us = 1000000.0 / calc_freq(f);
					t_n_us += round(floor(t_c_us / t_pulse_us) * t_pulse_us);
					f -= delta_f;
					while (micros() <= t_n_us);

					// ramp-down
					while (n < N) {
						//analogWriteFrequency(pwm_pin, f);
						n += 1;
						//if (n == N) t_n_us -= 500000 / f;
						if (n == N) t_n_us -= a_f_max / f;
						// calculate next time stamp:
						t_pulse_us = 1000000.0 / calc_freq(f);
						t_n_us += round(floor(delta_t_us / t_pulse_us) * t_pulse_us);
						f -= delta_f;
						// needed waiting time = next time stamp minus last time stamp:
						while (micros() <= t_n_us);
					}
					//pinMode(pwm_pin, OUTPUT);
				}

				uint32_t Numbers::micros()
				{
					return 2;
				}
			}
		}
	}
}


