#include <stdio.h>

// graphics register addresses

#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0xFF210000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0xFF210000))
#define GraphicsX1Reg   			(*(volatile unsigned short int *)(0xFF210002))
#define GraphicsY1Reg   			(*(volatile unsigned short int *)(0xFF210004))
#define GraphicsX2Reg   			(*(volatile unsigned short int *)(0xFF210006))
#define GraphicsY2Reg   			(*(volatile unsigned short int *)(0xFF210008))
#define GraphicsColourReg   		(*(volatile unsigned short int *)(0xFF21000E))
#define GraphicsBackGroundColourReg   	(*(volatile unsigned short int *)(0xFF210010))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/

#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);

// #defined constants representing values we write to the graphics 'command' register to get
// it to draw something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine		1
#define DrawVLine		2
#define DrawLine		3
#define	PutAPixel		0xA
#define	GetAPixel		0xB
#define	ProgramPaletteColour    0x10

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "Colours.h" contains constants for all 256 colours
// while the course file "ColourPaletteData.c" contains the 24 bit RGB data
// that is pre-programmed into the palette

#define	BLACK			0
#define	WHITE			1
#define	RED			2
#define	LIME			3
#define	BLUE			4
#define	YELLOW			5
#define	CYAN			6
#define	MAGENTA			7

/*******************************************************************************************
* This function writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/
void WriteAPixel(int x, int y, int Colour)
{
    // Deal with negative coordinates
    if (x < 0 || y < 0) {
        return;
    }

	WAIT_FOR_GRAPHICS;				// is graphics ready for new command

	GraphicsX1Reg = x;				// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;			// set pixel colour
	GraphicsCommandReg = PutAPixel;			// give graphics "write pixel" command
}

/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/

int ReadAPixel(int x, int y)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel;		// give graphics a "get pixel" command

	WAIT_FOR_GRAPHICS;			// is graphics done reading pixel
	return (int)(GraphicsColourReg) ;	// return the palette number (colour)
}


/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;        // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                // program green and blue into ls 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour; // issue command
}

// Draw a horizontal line from (x1,y1) to (x1+length-1, y1) of colour Colour
void HLine(int x1, int y1, int length, int Colour)
{
    // Deal with no length or negative length
    if (length <= 0) {
        return;
    }

    // Deal with negative y
    if (y1 < 0) {
        return;
    }

    // Deal with negative x
    if (x1 < 0) {
        if (x1 + length <= 0) {
            return;
        } else {
            x1 = 0;
            length -= -x1;
        }
    }

	int x2 = x1 + length; // We don't write to coordinate (x2,y1), but use it as a stopping point instead

    WAIT_FOR_GRAPHICS;              // is graphics ready for new command

    GraphicsX1Reg = x1;              // write coords to x1, y1, and x2, y2
    GraphicsY1Reg = y1;
    GraphicsX2Reg = x2;
    GraphicsY2Reg = y1;
    GraphicsColourReg = Colour;         // set pixel colour
    GraphicsCommandReg = DrawHLine;         // give graphics "draw horizontal line" command
}

// Draw a vertical line from (x1,y1) to (x1, y1+length-1) of colour Colour
void VLine(int x1, int y1, int length, int Colour)
{
    // Deal with no length or negative length
    if (length <= 0) {
        return;
    }

    // Deal with negative x
    if (x1 < 0) {
        return;
    }

    // Deal with negative y
    if (y1 < 0) {
        if (y1 + length <= 0) {
            return;
        } else {
            y1 = 0;
            length -= -y1;
        }
    }

    int y2 = y1 + length; // We don't write to coordinate (x1,y2), but use it as a stopping point instead

    WAIT_FOR_GRAPHICS;              // is graphics ready for new command

    GraphicsX1Reg = x1;              // write coords to x1, y1, and x2, y2
    GraphicsY1Reg = y1;
    GraphicsX2Reg = x1;
    GraphicsY2Reg = y2;
    GraphicsColourReg = Colour;         // set pixel colour
    GraphicsCommandReg = DrawVLine;         // give graphics "draw vertical line" command
}

// Draw a line from (x1,y1) to (x2,y2) of colour Colour
void Line(int x1, int y1, int x2, int y2, int Colour)
{
    WAIT_FOR_GRAPHICS;              // is graphics ready for new command

    GraphicsX1Reg = x1;              // write coords to x1, y1, and x2, y2
    GraphicsY1Reg = y1;
    GraphicsX2Reg = x2;
    GraphicsY2Reg = y2;
    GraphicsColourReg = Colour;         // set pixel colour
    GraphicsCommandReg = DrawLine;         // give graphics "draw line" command
}

// Draw a triangle of colour Colour that connects points (x1,y1), (x2,y2), and (x3, y3)
void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int Colour)
{
    Line(x1, y1, x2, y2, Colour);
    Line(x2, y2, x3, y3, Colour);
    Line(x3, y3, x1, y1, Colour);
}

// Draw a rectangle of colour Colour with a top left coordinate of (x1,y1) that is width pixels wide and height pixels tall
// The rectangle will be empty instead of filled
void Rectangle(int x1, int y1, int width, int height, int Colour)
{
    HLine(x1, y1, width, Colour);
    HLine(x1, y1+height-1, width, Colour);
    VLine(x1, y1, height, Colour);
    VLine(x1+width-1, y1, height, Colour);
}

// Draw a rectangle of colour Colour with a top left coordinate of (x1,y1) that is width pixels wide and height pixels tall
// The rectangle will be filled, instead of being empty
void FilledRectangle(int x1, int y1, int width, int height, int Colour)
{
    int i;
    for(i=y1; i < y1+height; i++) {
        HLine(x1, i, width, Colour);
    }
}

// Draw a rectangle with a top left coordinate of (x1,y1) that is width pixels wide and height pixels tall
// The rectangle will be filled with the colour Colour, instead of being empty
// The rectangle will have a border of width borderWidth and the border will have a colour of BorderColour
void FilledRectangleWithBorder(int x1, int y1, int width, int height, int borderWidth, int FillColour, int BorderColour)
{
    // Draw Border
    FilledRectangle(x1, y1, width, borderWidth, BorderColour); //Top
    FilledRectangle(x1, y1+height-borderWidth, width, borderWidth, BorderColour); //Bottom
    FilledRectangle(x1, y1, borderWidth, height, BorderColour); //Left
    FilledRectangle(x1+width-borderWidth, y1, borderWidth, height, BorderColour); //Right

    // Fill in
    FilledRectangle(x1+borderWidth, y1+borderWidth, width-2*borderWidth, height-2*borderWidth, FillColour);
}

int main(void)
{
    printf("Starting...\n");

    // Draw a box where:
    // the top line is red
    // right line is lime
    // bottom line is blue
    // left line is magenta
    HLine(150, 150, 150, RED);
    VLine(299, 150, 150, LIME);
    HLine(150, 299, 150, BLUE);
    VLine(150, 150, 150, MAGENTA);

    // Draw a cyan X inside the box
    Line(170,170, 279, 279, CYAN);
    Line(170,279, 279, 170, CYAN);

    // Draw another box 20 pixels to the right of the other box using Line instead of HLine and VLine
    Line(329, 150, 478, 150, RED);
    Line(478, 150, 478, 299, LIME);
    Line(329, 299, 478, 299, BLUE);
    Line(329, 150, 329, 299, MAGENTA);

    // Draw 4 red dots inside the new box using WriteAPixel
    WriteAPixel(402, 223, RED);
    WriteAPixel(404, 223, RED);
    WriteAPixel(402, 225, RED);
    WriteAPixel(404, 225, RED);

    // Draw a blue triangle
    Triangle(10, 10, 40, 40, 60, 20, BLUE);

    // Draw a yellow rectangle
    Rectangle(20, 300, 90, 20, YELLOW);

    // Draw a red solid rectangle
    FilledRectangle(300, 10, 200, 100, RED);

    // Draw a cyan rectangle with a 10px wide white border
    FilledRectangleWithBorder(300, 300, 50, 70, 10, CYAN, WHITE);

    printf("Done...\n");
    return 0 ;
}