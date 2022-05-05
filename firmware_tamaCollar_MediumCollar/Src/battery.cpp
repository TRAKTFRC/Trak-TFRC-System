#include <avr/io.h>
#include <util/delay.h>
#include "battery.h"

/*
Knowledge resource: https://wp.josh.com/2014/11/06/battery-fuel-guage-with-zero-parts-and-zero-pins-on-avr/
*/

// Returns the current Vcc voltage as a fixed point number with 1 implied decimal places, i.e.
// 50 = 5 volts, 25 = 2.5 volts,  19 = 1.9 volts
//
// On each reading we: enable the ADC, take the measurement, and then disable the ADC for power savings.
// This takes >1ms becuase the internal reference voltage must stabilize each time the ADC is enabled.
// For faster readings, you could initialize once, and then take multiple fast readings, just make sure to
// disable the ADC before going to sleep so you don't waste power. 

uint8_t readVccVoltage (void)
{
	
	// Select ADC inputs
	// bit    76543210 
	// REFS = 00       = Vcc used as Vref
	// MUX  =   100001 = Single ended, 1.1V (Internal Ref) as Vin
	
	ADMUX = 0b01001110;
			
	/*
	By default, the successive approximation circuitry requires an input clock frequency between 50
	kHz and 200 kHz to get maximum resolution.
	*/	
				
	// Enable ADC, set prescaller to /64 which will give a ADC clock of 8mHz/64 = 125kHz
	
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);
	
	/*
		After switching to internal voltage reference the ADC requires a settling time of 1ms before
		measurements are stable. Conversions starting before this may not be reliable. The ADC must
		be enabled during the settling time.
	*/
		
	_delay_ms(1);
				
	/*
		The first conversion after switching voltage source may be inaccurate, and the user is advised to discard this result.
	*/
	
		
	ADCSRA |= _BV(ADSC);				// Start a conversion


	while( ADCSRA & _BV( ADSC) ) ;		// Wait for 1st conversion to be ready…
										//..and ignore the result


	// Second conversion after 1mS delay						
	_delay_ms(1);

	ADCSRA |= _BV(ADSC);				// Start a conversion

	while( ADCSRA & _BV( ADSC) ) ;		// Wait for 2nd conversion

	_delay_ms(10);


	/*
		After the conversion is complete (ADIF is high), the conversion result can be found in the ADC
		Result Registers (ADCL, ADCH).		
		
		When an ADC conversion is complete, the result is found in these two registers.
		When ADCL is read, the ADC Data Register is not updated until ADCH is read.		
	*/
	
	// Note we could have used ADLAR left adjust mode and then only needed to read a single byte here

	uint8_t low  = ADCL;
	uint8_t high = ADCH;

//	high = 0;
	//printf ("ADCL: %d, ADCH: %d", low, high);

	uint16_t adc = (high << 8) | low;		// 0<= result <=1023
			
	//printf ("ADC: %lu\r\n", (unsigned long) adc);
	// Compute a fixed point with 1 decimal place (i.e. 5v= 50)
	//
	// Vcc   =  (1.1v * 1024) / ADC
	// Vcc10 = ((1.1v * 1024) / ADC ) * 10				->convert to 1 decimal fixed point
	// Vcc10 = ((11   * 1024) / ADC )				->simplify to all 16-bit integer math
				
	uint8_t vccx10 = (uint8_t) ( (11 * 1024) / adc); 
	
	printf ("vccx10: %d\r\n", vccx10);
	/*	
		Note that the ADC will not automatically be turned off when entering other sleep modes than Idle
		mode and ADC Noise Reduction mode. The user is advised to write zero to ADEN before entering such
		sleep modes to avoid excessive power consumption.
	*/
	
	ADCSRA &= ~_BV( ADEN );			// Disable ADC to save power
	
	return vccx10;
}

float readVccVoltageFloat (void)
{
	return (readVccVoltage()/10);
}