/**
 * A minimal example how to push data (parallel) to 8bit shift register with ATTiny13A
 * The example is an binary counter and outputs data from 0 to 255
 *
 * Port assignments
 * `-> SER:   PB0 // Port to transmit data
 * `-> SRCLK: PB1 // Clock port for data transmission
 * `-> RCLK:  PB4 // Latch port. Show data.
 *
 * @author Igor Martens
 * @since 14.09.2018
 */

#define F_CPU 9600000UL

#include <avr/io.h>
#include <util/delay.h>

void setup() {
    // Setup output ports
    DDRB |= (1<<DDB0);  // SER Port
    DDRB |= (1<<DDB1);  // SRCLK Port
    DDRB |= (1<<DDB4);  // RCLK Port

    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
    PORTB &= ~(1 << PB4);
}

/**
 * Push the given byte to the register and finally execute latch.
 * The left bit will be pushed first.
 */
void pushByteAndLatch(uint8_t byte) {
    for (uint8_t i=0; i<8; ++i) {
        // If the left bit is 1 then set Port B to HIGH, LOW otherwise
        (byte & 128) ? PORTB |= (1 << PB0) : PORTB &= ~(1 << PB0);

        // Set Schift Clock on and off to write data from Port B0 to the register
        PORTB |= (1 << PB1);
        PORTB &= ~(1 << PB1);

        // Shift the data bytes to the left to get the next data bit
        byte = byte << 1;
    }

    // Latch the register data to get it on the outputs
    PORTB |= (1 << PB4);
    PORTB &= ~(1 << PB4);
}

/**
 * Set the data inside register to zero and latch
 * This we only need to save one port on the MCU.
 * If the MCU have enough ports SRCLR on the shift register can be used to clear data instead of this method.
 */
void clearRegister() {
    pushByteAndLatch(0b00000000);
}

int main(void) {
    setup();
    clearRegister();

    while(1) {
        // Output on the Register binary value from 0 to 255 (from 0b00000000 to 0b11111111)
        for (uint8_t i=0; i<255; ++i) {
            pushByteAndLatch(i);
            _delay_ms(1000);
        }
    }

    return 0;
}
