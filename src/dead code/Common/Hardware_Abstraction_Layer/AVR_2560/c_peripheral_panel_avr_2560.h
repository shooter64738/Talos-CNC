/*
* c_peripheral_panel_avr_2560.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __CONTROL_TYPE_COORDINATOR_H__
#define __CONTROL_TYPE_COORDINATOR_H__

//#define BANK1_INPUT_PINS PINB
//#define BANK2_INPUT_PINS PINK
//
//#define BLOCK_SKIP_BIT 0 //a15
//#define SINGLE_BLOCK_BIT 1 //a14
//#define SPINDLE_ON_OF_BIT 2 //a13
//#define COOLANT_ON_OFF_BIT 3 //a12
//#define RAPID_RATE_SELECT_BIT 4 //a11
//#define JOG_AXIS_SELECT_BIT 5 //a10
//#define JOG_SCALE_SELECT 6 //a09
//#define V_AXIS_PULSE_BIT BIT(7) //a08

// Buttons connected to PA0 and PA1
#define BUTTON_PORT  PORTK
#define BUTTON_PIN   PINK
#define BUTTON_DDR   DDRK

#define JOG_SCALE (1<<PB0)		//Cycles the scale of each mpg wheel tick. 1, .1, .01, .001
#define JOG_AXIS (1<<PB1)		//Cycles the axis that is jogged,
								// X(0), Y(1), Z(2), A(3), B(4), C(5), U(7), V(8), {W(9) not implimented}
#define OVERRIDE_RAPID (1<<PB2) //Cycles the rapid over ride % 0=0%, 25=25% slower, 50 = 50% slower
#define BLOCK_SKIP (1<<PB3)		//Determines if g code blocks begining with the skip char (/) are executed or not
#define SINGLE_BLOCK (1<<PB4)	//Tells the motion processor to only run 1 block (line) at a time. Each line 
								//must be started with the cycle start button
#define SPINDLE (1<<PB5)		//If the spindle is off, this turns it on in the direction it was last programmed to
								//turn. If the spindle is on this will turn it off
#define COOLANT (1<<PB6)		//If the coolant is off, this turns it on in the mode (mist/flood) it was last
								//programmed to run in. If the coolant is on this will turn it off
#define SHIFT_MODE (1<<PB7)		//When pressed, each button will have a secondary function.
#define BUTTON_MASK  (JOG_SCALE | JOG_AXIS | OVERRIDE_RAPID | BLOCK_SKIP | SINGLE_BLOCK | SPINDLE | COOLANT | SHIFT_MODE)


// Variable to tell that the button is pressed (and debounced).
// Can be read with button_down() which will clear it.
extern volatile uint8_t buttons_down;


// Decrease 2 bit vertical counter where mask = 1.
// Set counters to binary 11 where mask = 0.
#define VC_DEC_OR_SET(high, low, mask)		\
low = ~(low & mask);			\
high = low ^ (high & mask)

// Check button state and set bits in the button_down variable if a
// debounced button down press is detected.
// Call this function every 10 ms or so.
static inline void debounce(void)
{
	// Eight vertical two bit counters for number of equal states
	static uint8_t vcount_low  = 0xFF, vcount_high = 0xFF;
	// Keeps track of current (debounced) state
	static uint8_t button_state = 0;

	// Read buttons (active low so invert with ~). Xor with
	// button_state to see which ones are about to change state
	uint8_t state_changed = ~BUTTON_PIN ^ button_state;

	// Decrease counters where state_changed = 1, set the others to 0b11.
	VC_DEC_OR_SET(vcount_high, vcount_low, state_changed);
	
	// Update state_changed to have a 1 only if the counter overflowed
	state_changed &= vcount_low & vcount_high;
	// Change button_state for the buttons who's counters rolled over
	button_state ^= state_changed;

	// Update button_down with buttons who's counters rolled over
	// and who's state is 1 (pressed)
	buttons_down |= button_state & state_changed;
}
namespace Hardware_Abstraction_Layer
{

	class PeripheralPanel
	{
		//variables
		public:
		
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void _configure_debounce_timer();
		static uint32_t check_panel_input();
		static uint8_t key_pressed(uint8_t button_mask);
		
		protected:
		private:
		static void _configure_switch_pins_bank_1();
		static void _configure_switch_pins_bank_2();
		static void _debounce_init(void);

		
		
	}; //control_type_coordinator
};
#endif //__CONTROL_TYPE_COORDINATOR_H__
#endif