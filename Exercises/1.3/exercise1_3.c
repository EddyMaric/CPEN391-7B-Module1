#include <stdio.h>

#define RS232_ReceiverFifo (*(volatile unsigned char *)(0xFF210200))
#define RS232_TransmitterFifo (*(volatile unsigned char *)(0xFF210200))
#define RS232_InterruptEnableReg (*(volatile unsigned char *)(0xFF210202))
#define RS232_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210204))
#define RS232_FifoControlReg (*(volatile unsigned char *)(0xFF210204))
#define RS232_LineControlReg (*(volatile unsigned char *)(0xFF210206))
#define RS232_ModemControlReg (*(volatile unsigned char *)(0xFF210208))
#define RS232_LineStatusReg (*(volatile unsigned char *)(0xFF21020A))
#define RS232_ModemStatusReg (*(volatile unsigned char *)(0xFF21020C))
#define RS232_ScratchReg (*(volatile unsigned char *)(0xFF21020E))
#define RS232_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210200))
#define RS232_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210202))

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
    // This is for baudrate of 9600
    RS232_DivisorLatchLSB = 0x45; // Paul Davies told me to do this idk what these constants mean
    RS232_DivisorLatchMSB = 0x01;

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

int main(void)
{
    Init_RS232();
    RS232Flush();

    if(!testFlush()) {
        printf("Failed testFlush.\n");
        return 0;
    } else {
        printf("Passed testFlush.\n");
    }

    if(!testWrite()) {
        printf("Failed testWrite.\n");
        return 0;
    } else {
        printf("Passed testWrite.\n");
    }

    if (!testRead()) {
        printf("Failed testRead.\n");
        return 0;
    } else {
        printf("Passed testRead.\n");
    }

    printf("Passed all tests.\n");
    return 0;
}