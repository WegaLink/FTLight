//////////////////////////////////////////////////////////////////////////////
//
// CmGraphics.h: Declaration of the CmGraphics classes
//
//////////////////////////////////////////////////////////////////////////////
//
// Author: 	   Eckhard Kantz
// website:    http://wegalink.eu
//
//////////////////////////////////////////////////////////////////////////////
/*
This is FREE software

Permission is hereby granted, free of charge,  to any person obtaining  a copy
of this software and associated documentation files (the "Software"),  to deal
in the Software without restriction, including without limitation  the  rights
to use,  copy,  modify,  merge,  publish,  distribute, sublicense, and/or sell
copies  of  the  Software,   and  to  permit  persons  to  whom  the  Software
is furnished to do so, subject to the following conditions:

There are no conditions imposed on the use of this software.

THE SOFTWARE IS PROVIDED "AS IS",  WITHOUT  WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING  BUT  NOT  LIMITED  TO  THE  WARRANTIES OF MERCHANTABILITY,
FITNESS  FOR  A  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS  OR  COPYRIGHT  HOLDERS  BE  LIABLE  FOR  ANY CLAIM,  DAMAGES OR OTHER
LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,  ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN  THE
SOFTWARE.
*/

#ifndef CmDrawingsH
#define CmDrawingsH

// Drawing components
#include "Services/Drawing/CmChart.h"
#include "Services/Drawing/CmFocus.h"

// legacy base dependencies
#define WIN32_MFC
#include "Services/Basic/CmTime.h"
#include "Services/Basic/CmTypes.h"
#include "Services/Basic/CmString.h"



//#include "fft.h"

namespace Cosmos
{

// Distance categories used in 4D views ranging from sub-atom to beyound observable universe
enum MDistanceCategory{
		M_UNIVERSE,
		M_GALAXY,
		M_USUAL,
		M_ATOM,
};

// Class for testing all graphics functionality
class CmGraphics 
{
public:
	// Unit test function
	int32 unitTest();

	// Constructor and destructor
	CmGraphics();
	virtual ~CmGraphics();
};

enum BMP4_COLOR {cWhite   = 0,
				 cGray1   = 1,
				 cGray2   = 2,
				 cGray3   = 3,
				 cGray4   = 4,
				 cGray5   = 5,
				 cGray6   = 6,
				 cBlack   = 7,
				 cRed     = 8,
				 cOrange  = 9,
				 cYellow  = 10,
				 cLemon   = 11,
				 cGreen   = 12,
				 cCyan    = 13,
				 cBlue    = 14,
				 cMagenta = 15};


enum CM_COLOR
{
  CmBlack     = 0x000000,
  CmRed       = 0xFF0000,
  CmGreen     = 0x00FF00,
  CmBlue      = 0x0000FF,
  CmWhite     = 0xFFFFFF,

};

// Basic RGB structure that defines a pixel on a display
typedef union
{
  struct
  {
    uint8	Red;
    uint8	Green;
    uint8	Blue;
    uint8	Alpha;
  } RGBA;
  uint32  Color;
} CmPixel;


/**
 *  CmCanvas.
 *  The CmCanvas provides for RGB drawing capabilities. It offers painting
 *  functions for basic geometric objects like a dot, a line, a circle and more.
 *  Actual functionality will be provided either by GDI or DX9 or OpenGL.
 */
class CmCanvas
{
public:
	// Resize and draw
	virtual bool resizeChart(CmSize2D NewSize) =0;
	virtual void drawChart(CmSize2D Size) =0;
	// Access parameters;
	uint32 getChartWidth();
	uint32 getChartHeight();
	// Draw elements
	void clearCanvas(CmPixel Color);
	void drawLine(int32 X1,int32 Y1,int32 X2,int32 Y2, CmPixel Color, int32 LineWidth =1);

	// Constructor and destructor
	CmCanvas(HWND hWnd, CmSize2D CanvasSize);
	virtual ~CmCanvas();

protected:
	// Parameters set when opening a new canvas
	HWND hWnd;
	HDC CanvasDC;
	int CanvasWidth;
	int CanvasHeight; 
  
	// Chart data and geometry
	uint32 *ChartRGB;
	int32 ChartWidth;
	int32 ChartHeight;
	int32 ChartPitch;
	int32 ChartSize;
	int32 ChartPixels;
	int32 ChartMemWidth;
	int32 ChartMemHeight;


	friend class CmSignalScreen;

private:
	// Pixel data
	CmPixel *Pixel;
};

// Class for generating 4-bit bitmaps
class CmBmp4
{
public:
	// Access parameters;
	void setSize(uint32 nImageWidth,uint32 nImageHeight);
	uint32 getWidth();
	uint32 getHeight();
	// Adjust colors
	void   setColor(BMP4_COLOR uColor,uint32 uValue);
	uint32 getColor(BMP4_COLOR uColor);
	// Draw elements
	void clearBitmap(int32 nColor);
	void drawLine(int32 nX1,int32 nY1,int32 nX2,int32 nY2,BMP4_COLOR uColor =cBlack,int32 nWidth =1);
	void drawVerticalRainbowLine(int32 nX1,int32 nY1,int32 nX2,int32 nY2,BMP4_COLOR nStartColor =cRed,BMP4_COLOR nEndColor =cMagenta,int32 nWidth =1);

	// Generate bitmap file
	int32  writeBitmapFile();
	uint8* writeValue(uint8* pValue8, void* pValue,int32 nCount =4);

	// Constructor and destructor
	CmBmp4();
	~CmBmp4();

	// Public variables
	CmString  FileName;

protected:
	int32	ImageWidth;
	int32	ImageHeight;

	friend	class CmSignalScreen;

private:
	// Internal functions
	int32	getBytesPerRow(int32 nWidth,int32 nBitCount);
	// Settings
	int32	BytesPerRow;
	// File header
	int8	FileType[2];
	int32	FileSize;
	int32	FileReserved;
	int32	FileOffBits;
	// Image header
	int32	ImageSize;
	int16	ImagePlanes;
	int16	ImageBitCount;
	int32	ImageCompression;
	int32	ImageSizeImage;
	int32	ImageXPixelsPerMeter;
	int32	ImageYPixelsPerMeter;
	int32	ImageColorsUsed;
	int32	ImageColorsImportant;
	// Color table
	uint32*	pColorTable;
	// Pixel data
	uint8*	pPixel;
	// Bitmap
	CmString mBitmap;
};

// Bitmap with a grid to draw signal curves on it
class CmSignalScreen : public CmBmp4
{
public:
	// Initialize signal screen
	uint32 setAxisX(uint64 u64Left,uint64 u64Period);
	void   setZoomX(uint64 u64Zoom,uint64 u64DivisionZoom,uint32 uWidthZoom);
	void   setShortTermX(uint64 u64ShortTerm,uint64 u64DivisionShortTerm,uint32 uWidthShortTerm);
	void   setLongTermX(uint64 u64LongTerm,uint64 u64DivisionLongTerm,uint32 uWidthLongTerm);
	uint32 setAxisY(uint64 u64Start,uint64 u64End,uint64 u64Division =0);
	// Access parameters
	uint64 getZoomX();
	uint64 getShortTermX();
	uint64 getLongTermX();
	// Draw functions
	void clearSignalScreen(BMP4_COLOR uColor);
	void generateGrid();
	void plotSignal(uint64 u64X,double u64Y,BMP4_COLOR uColor =cOrange,int64 u64Offset =0, uint32 uLineWidht =1);
	void plotFFT(CmString& CmDataXVector);
	// Constructor and destructor
	CmSignalScreen();
	~CmSignalScreen();
private:
	// nanosecond-based time values
	uint64	u64LeftX;
	uint64	u64PeriodX;
	uint64	u64ZoomX;
	uint64	u64ShortTermX;
	uint64	u64LongTermX;
	uint64	u64DivisionZoomX;
	uint64	u64DivisionShortTermX;
	uint64	u64DivisionLongTermX;
	uint32  uWidthZoomX;
	uint32  uWidthShortTermX;
	uint32  uWidthLongTermX;
	// scaled time values
	uint32	uShiftX;		// scaling factor
	uint64  u64LeftX1;
	uint64  u64ZoomX1;
	uint64  u64ShortTermX1;
	uint64  u64LongTermX1;
	// original Y values
	uint64	u64LowY;
	uint64	u64HighY;
	uint64	u64DivisionY;
	// scaled Y values
	uint32	uShiftY;		// scaling factor
	uint64  u64LowY1;
	uint64  u64RangeY1;
	// initialization for first plot point
	bool	fFirst[16];
	uint32	uLastX[16];
	uint32	uLastY[16];
	// FFT result array
	int32	FFT[2048];
//	complex fft[1024];
};


#ifdef NODEF
// CmPos represents the latitude, longitude and distance to a point in the universe.
// An observable object is usually made from many points spanning triangular areas.
class CmPos : public MNode  
{
public:
	//Access functions
	MTime* setTime(MTime* pTime);
	MTime* getTime();
	double getDistance();
	double getLongitude();
	double getLatitude();
	//Mark pointers which are used as values
	virtual bool isLessPtr();
	virtual bool isMorePtr();
	virtual bool isNextPtr();
	virtual bool isDataPtr();
	//Report special features and runtime type 
	bool isAttachement();
	MNodeType getNodeType();
	//Constructor and destructor
	CmPos();
	virtual ~CmPos();

};


// CmView is the direction and rotation of an observer.
// Changing the view parameter lets you look into different directions but from the same 
// observer point of view.
class CmView : public MNode  
{
public:
	//Access functions
	CmPos* setPos(CmPos* pPos);
	CmPos* getPos();
	double getAzimuth();
	double getAltitude();	
	double getAperture();
	double getRotation();
	unsigned getIncAngle();
	//Mark pointers which are used as values
	virtual bool isLessPtr();
	virtual bool isMorePtr();
	virtual bool isNextPtr();
	virtual bool isDataPtr();
	//Report special features and runtime type 
	bool isAttachement();
	MNodeType getNodeType();
	//Constructor and destructor
	CmView();
	virtual ~CmView();

};



// CmScreen decides about the graphical representaion of an observed object on screen,
// e.g. by changing the aperture a zoom effect will be accomplished.
class CmScreen : public MNode  
{
public:
	//Methods related to 4D views
	unsigned getScreenHeight();
	unsigned getScreenWidth();
	unsigned getPointerY();
	unsigned getPointerX();
	void setScreenSize(unsigned, unsigned);
	void setPointer(unsigned, unsigned);
	//Access functions
	CmView* setView(CmView* pView);
	CmView* getView();
	//Mark pointers which are used as values
	virtual bool isLessPtr();
	virtual bool isMorePtr();
	virtual bool isNextPtr();
	virtual bool isDataPtr();
	//Report special features and runtime type 
	bool isAttachement();
	MNodeType getNodeType();
	//Constructor and destructor
	CmScreen();
	virtual ~CmScreen();

private:

};

// CmColor handels color usage in screens
class CmColor : public MNode
{
public:
	//Access functions
	void setColor(unsigned uColor);
	unsigned getColor();

	//Report special features and runtime type 
	bool isAttachement();
	MNodeType getNodeType();
	//Constructor and destructor
	CmColor();
	virtual ~CmColor();
};

class CmDisplay
{
public:
	CmDisplay();
	~CmDisplay();

	// Setup a new display size
	void setDisplaySize(unsigned uWidth,unsigned uHeight);
	unsigned getWidth();
	unsigned getHeigth();
	// Access pixel array
	void setPixel(unsigned uX,unsigned uY,CmPixel mPixel);
	void setFluxPixel(unsigned uX,unsigned uY,unsigned uFluxPixel);
	CmPixel getPixel(unsigned uX,unsigned uY);
	void initSpectrum();

	friend class MWinDisplay;
	BYTE*		pPixel;
	unsigned	uScan;		
protected:

private:
	unsigned	uWidth;
	unsigned	uHeight;

};

#endif

}  // namespace
#endif // ifndef CmDrawingsH
