#include <stdio.h>

// ALL parallel IO ports created by QSYS have a 32 bit wide interface as far as the processor,
// that is, it reads and writes 32 bit data to the port, even though the
// port itself might only be configures as an 8 or 10 or even 4 bit wide port
//
// To access the port and read switches and writes to leds/hex displays etc, we set
// up "int pointer", i.e. pointers to 32 bit data
// to read/write data from/to the port and discard any upper bits we don't need.
// in the case of reading the switches and copying to leds/hex displays, it doesn't matter as
// they are all 8 bits so the upper unused bit don't matter,
// but push button port is 4 bit input port so reading with will give us 28 bits of leading 0's
// followed by the 4 bits corresponding to the push buttons in bits 0-3 of the data we read fom port

// the addresses below were defined by us when we created our system with Qys and assigned
// addresses to each of the ports we added (open up Qsys and check the Address Tab if you are uncertain)

#define SWITCHES    (volatile unsigned int *)(0xFF200000)
#define PUSHBUTTONS (volatile unsigned int *)(0xFF200010)

#define LEDS        (volatile unsigned int *)(0xFF200020)
#define HEX0_1      (volatile unsigned int *)(0xFF200030)
#define HEX2_3      (volatile unsigned int *)(0xFF200040)
#define HEX4_5      (volatile unsigned int *)(0xFF200050)


void main(void)
{
    int pushbuttons;
    int hex_0_1_to_write;
    int hex_2_3_to_write;

	printf("Exercise 1.1\n");

    while(1) {
        pushbuttons = *PUSHBUTTONS;
        hex_0_1_to_write = 0x0;
        hex_2_3_to_write = 0x0;

        if ((pushbuttons >> 0) & 1) {
            hex_0_1_to_write += 0x8;
        }

        if ((pushbuttons >> 1) & 1) {
            hex_0_1_to_write += 0x80;
        } else {
            hex_0_1_to_write += 0x10;
        }

        if ((pushbuttons >> 2) & 1) {
            hex_2_3_to_write += 0x8;
        } else {
            hex_2_3_to_write += 0x2;
        }

        if ((pushbuttons >> 3) & 1) {
            hex_2_3_to_write += 0x80;
        } else {
            hex_2_3_to_write += 0x30;
        }

        *HEX0_1 = hex_0_1_to_write;
        *HEX2_3 = hex_2_3_to_write;```
    }
}