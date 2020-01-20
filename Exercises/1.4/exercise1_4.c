#include <stdio.h>

/* START OF SERIAL PORT CODE */

#define RS232_ReceiverFifo (*(volatile unsigned char *)(0xFF210230))
#define RS232_TransmitterFifo (*(volatile unsigned char *)(0xFF210230))
#define RS232_InterruptEnableReg (*(volatile unsigned char *)(0xFF210232))
#define RS232_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210234))
#define RS232_FifoControlReg (*(volatile unsigned char *)(0xFF210234))
#define RS232_LineControlReg (*(volatile unsigned char *)(0xFF210236))
#define RS232_ModemControlReg (*(volatile unsigned char *)(0xFF210238))
#define RS232_LineStatusReg (*(volatile unsigned char *)(0xFF21023A))
#define RS232_ModemStatusReg (*(volatile unsigned char *)(0xFF21023C))
#define RS232_ScratchReg (*(volatile unsigned char *)(0xFF21023E))
#define RS232_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210230))
#define RS232_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210232))

#define BRClkFrequency 50000000
#define DesiredBaudrate 9600

#define RS232_LineControlReg_WordLengthSelect0 0
#define RS232_LineControlReg_WordLengthSelect1 1
#define RS232_LineControlReg_DivisorLatchAccessBit 7

#define RS232_FifoControlReg_ReceiveFIFOReset 1
#define RS232_FifoControlReg_TransmitFIFOReset 2

#define RS232_LineStatusReg_DataReady 0
#define RS232_LineStatusReg_TransmitterHoldingRegister 5

/**************************************************************************
 Subroutine to initialise the RS232 Port by writing some data
 to the internal registers.
 Call this function at the start of the program before you attempt
 to read or write to data via the RS232 port


 Refer to UART data sheet for details of registers and programming
***************************************************************************/
void Init_RS232(void)
{
 // set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
    RS232_LineControlReg = (1 << RS232_LineControlReg_DivisorLatchAccessBit);

 // set Divisor latch (LSB and MSB) with correct value for required baud rate
    int baud_rate_divisor_value = BRClkFrequency / (DesiredBaudrate * 16);
    RS232_DivisorLatchLSB = baud_rate_divisor_value & 0xffff ;
    RS232_DivisorLatchMSB = baud_rate_divisor_value >> 8;

 // set bit 7 of Line control register back to 0 and
 // program other bits in that reg for 8 bit data, 1 stop bit, no parity etc
    RS232_LineControlReg = (1 << RS232_LineControlReg_WordLengthSelect0) + (1 << RS232_LineControlReg_WordLengthSelect0);

 // Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
    RS232_FifoControlReg = (1 << RS232_FifoControlReg_ReceiveFIFOReset) + (1 << RS232_FifoControlReg_TransmitFIFOReset);

 // Now Clear all bits in the FiFo control registers
    RS232_FifoControlReg = 0;
}

// the following function polls the UART to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read from the FIFO
int RS232TestForReceivedData(void)
{
 // if RS232_LineStatusReg bit 0 is set to 1
 //return TRUE, otherwise return FALSE
    return ((RS232_LineStatusReg >> RS232_LineStatusReg_DataReady) & 1) == 1;
}

int putcharRS232(int c)
{
 // wait for Transmitter Holding Register bit (5) of line status register to be '1'
 // indicating we can write to the device
    while (((RS232_LineStatusReg >> RS232_LineStatusReg_TransmitterHoldingRegister) & 1) == 0) {
    }

 // write character to Transmitter fifo register
    RS232_TransmitterFifo = c;

 // return the character we printed
    return c;
}

int getcharRS232( void )
{
 // wait for Data Ready bit (0) of line status register to be '1'
    while(!RS232TestForReceivedData()) {
    }

 // read new character from ReceiverFiFo register
 // return new character
    return RS232_ReceiverFifo;
}

//
// Remove/flush the UART receiver buffer by removing any unread characters
//
void RS232Flush(void)
{
 // while bit 0 of Line Status Register == ‘1’
 // read unwanted char out of fifo receiver buffer
    while(RS232TestForReceivedData()) {
        int read = RS232_ReceiverFifo;
        read += 1;
    }

    return; // no more characters so return
}

int testWrite(void) {

    printf("Starting testWrite.\n");

    // 11 Characters + 1 null terminating character
    char word[12] = "Exercise1.3";

    printf("Wrote %s to serial port.\n", word);

    int i;
    for(i = 0; i < 11; i++) {
        putcharRS232(word[i]);
    }

    printf("Done testWrite.\n");

    return RS232TestForReceivedData();
}

int testRead(void) {

    printf("Starting testRead.\n");

    char expected[12] = "Exercise1.3";

    char output[12];
    int j;
    for(j = 0; j < 11; j++) {
        output[j] = getcharRS232();
        if(output[j] != expected[j]) {
            return 0;
        }
    }
    output[11] = '\0';

    printf("Output is: %s.\n", output);

    printf("Done testRead.\n");

    return 1;
}

int testFlush(void) {

    printf("Starting testFlush.\n");

    while (RS232TestForReceivedData()) {
        getcharRS232();
    }

    putcharRS232('a');

    while(!RS232TestForReceivedData()) {
        // wait for character to be ready
    }

    RS232Flush();

    printf("Done testFlush.\n");

    return !RS232TestForReceivedData();
}

/* END OF SERIAL PORT CODE */

/* START OF TOUCHSCREEN CODE */



/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void Init_Touch(void)
{
 // Program serial port to communicate with touchscreen
 // send touchscreen controller an "enable touch" command

	// Send the touch enable command
	putcharRS232(0x55);
	printf("Put first char\n");
	putcharRS232(0x1);
	printf("Put second char\n");
	putcharRS232(0x12);
	printf("Put third char\n");

	int i = 0;
	printf("Got byte %d: %x\n", i++, getcharRS232());
	printf("Got byte %d: %x\n", i++, getcharRS232());
	printf("Got byte %d: %x\n", i++, getcharRS232());
	printf("Got byte %d: %x\n", i++, getcharRS232());
}


// /*****************************************************************************
// ** test if screen touched
// *****************************************************************************/
// int ScreenTouched( void )
// {
//  // return TRUE if any data received from serial pory connected to
//  // touchscreen or FALSE otherwise
// }
// /*****************************************************************************
// ** wait for screen to be touched
// *****************************************************************************/
// void WaitForTouch()
// {
// while(!ScreenTouched())
//  ;
// }
// /* a data type to hold a point/coord */
// typedef struct { int x, y; } Point ;
// ****************************************************************************
// * This function waits for a touch screen press event and returns X,Y coord
// ****************************************************************************
// Point GetPress(void)
// {
//  Point p1;
//  // wait for a pen down command then return the X,Y coord of the point
//  // calibrated correctly so that it maps to a pixel on screen
//  return p1;
// }
// /*****************************************************************************
// * This function waits for a touch screen release event and returns X,Y coord
// *****************************************************************************/
// Point GetRelease(void)
// {
//  Point p1;
//  // wait for a pen up command then return the X,Y coord of the point
//  // calibrated correctly so that it maps to a pixel on screen
//  return p1;
// }

/* END OF TOUCHSCREEN CODE */

/* START OF TEST CODE */

int main(void)
{
    Init_RS232();
    RS232Flush();
    Init_Touch();

    printf("Done.\n");
    return 0;
}

/* END OF TEST CODE */