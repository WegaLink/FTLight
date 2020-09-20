//////////////////////////////////////////////////////////////////////////////
//
// CmGraphics.cpp: Implementation of the CmGraphics classes
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

#include "Services/Drawing/CmDrawing.h"

using namespace Cosmos;

//////////////////////////////////////////////////////////////////////
// CmCanvas Class
//////////////////////////////////////////////////////////////////////
CmCanvas::CmCanvas(HWND hWnd, CmSize2D CanvasSize)
{
  // Save parameters
  this->hWnd = hWnd;
  CanvasWidth = (int)CanvasSize.x();
  this->CanvasHeight = (int)CanvasSize.y();

  // Obtain a device context for received windows handle
  CanvasDC = GetDC(hWnd);

  // Initialize variables
  ChartWidth = 0;
  ChartHeight = 0;

}

CmCanvas::~CmCanvas()
{
  // Release window's device context
  if( CanvasDC != 0 )
    ReleaseDC(hWnd,CanvasDC);

}

uint32 CmCanvas::getChartWidth()
{
  return ChartWidth;
}

uint32 CmCanvas::getChartHeight()
{
  return ChartHeight;
}

//////////////////////////////////////////////////////////////////////
// CmGraphics Class
//////////////////////////////////////////////////////////////////////
CmGraphics::CmGraphics()
{
}

CmGraphics::~CmGraphics()
{
}

int32 CmGraphics::unitTest()
{
	CmBmp4 mBmp4;

	// Write default bitmap
	mBmp4.writeBitmapFile();
	// Clear bitmap
	mBmp4.clearBitmap(0);
	mBmp4.FileName = "Bmp4-1.bmp";
	mBmp4.writeBitmapFile();
	// Draw lines
	mBmp4.drawLine(100,100,300,300,cBlack,10);
	mBmp4.drawLine(100,300,300,100,cBlack,10);
	// Quadrat
	mBmp4.drawLine(500,100,700,100);
	mBmp4.drawLine(700,100,700,300);
	mBmp4.drawLine(700,300,500,300);
	mBmp4.drawLine(500,300,500,100);
	// Rhomb
	mBmp4.drawLine(530,100,700,130,cBlue,5);
	mBmp4.drawLine(700,130,670,300,cBlue,5);
	mBmp4.drawLine(670,300,500,270,cBlue,5);
	mBmp4.drawLine(500,270,530,100,cBlue,5);
	// Rhomb
	mBmp4.drawLine(670,100,700,270,cRed,5);
	mBmp4.drawLine(700,270,530,300,cRed,5);
	mBmp4.drawLine(530,300,500,130,cRed,5);
	mBmp4.drawLine(500,130,670,100,cRed,5);
	// Slope
	mBmp4.drawLine(400,250,600,350);
	mBmp4.drawLine(400,030,030,100,cOrange,30);
	mBmp4.drawLine(400,030,770,100,cOrange,30);
	// Polygon
	mBmp4.drawLine(000,401,100,400,cBlack);
	mBmp4.drawLine(100,400,200,449,cOrange);
	mBmp4.drawLine(200,449,390,000,cYellow);
	mBmp4.drawLine(390,000,400,425,cLemon);
	mBmp4.drawLine(400,425,500,435,cGreen);
	mBmp4.drawLine(500,435,600,380,cCyan);
	mBmp4.drawLine(600,380,700,440,cBlue);
	mBmp4.drawLine(700,440,799,400,cMagenta);
	mBmp4.FileName = "Bmp4-2.bmp";
	mBmp4.writeBitmapFile();

	printf("\nMGraphics unitTest() finished\n");
	return 0;
}


//////////////////////////////////////////////////////////////////////
// CmBmp4 Class
//////////////////////////////////////////////////////////////////////
CmBmp4::CmBmp4()
{
	// Prefill file header
	FileType[0]		= 'B';
	FileType[1]		= 'M';
	FileReserved	= 0;
	FileOffBits		= 118;
	// Prefill image header
	ImageSize				= 40;
	ImageWidth				= 500;
	ImageHeight				= 300;
	ImagePlanes				= 1;
	ImageBitCount			= 4;
	ImageCompression		= 0;
	ImageSizeImage			= 0;
	ImageXPixelsPerMeter	= 2835;
	ImageYPixelsPerMeter	= 2835;
	ImageColorsUsed			= 16;
	ImageColorsImportant	= 16;
	// Calculate values and obtain memory
	setSize(ImageWidth,ImageHeight);
	FileName	= "Bmp4.bmp";
}

CmBmp4::~CmBmp4()
{
}

void CmBmp4::setSize(uint32 nImageWidth,uint32 nImageHeight)
{
	// Initialize bitmap memory
	ImageWidth  = nImageWidth;
	ImageHeight = nImageHeight;
	BytesPerRow = getBytesPerRow(ImageWidth,ImageBitCount);
	FileSize	= 118 + BytesPerRow * ImageHeight;
	mBitmap.setLength(FileSize);
	pColorTable	= (uint32*)(mBitmap.getBuffer() + FileOffBits - 64);
	pPixel		= (uint8*) (mBitmap.getBuffer() + FileOffBits);
	// Prefill color table
	*(pColorTable+0)  =0x00FFFFFF;		// white
	*(pColorTable+1)  =0x00C8C8C8;		// gray1
	*(pColorTable+2)  =0x00A0A0A0;		// gray2
	*(pColorTable+3)  =0x00808080;		// gray3
	*(pColorTable+4)  =0x00606060;		// gray4
	*(pColorTable+5)  =0x00404040;		// gray5
	*(pColorTable+6)  =0x00202020;		// gray6
	*(pColorTable+7)  =0x00000000;		// black
	*(pColorTable+8)  =0x00E00000;		// red
	*(pColorTable+9)  =0x00FF8000;		// orange
	*(pColorTable+10) =0x00FFFF00;		// yellow
	*(pColorTable+11) =0x0090FF00;		// lemon
	*(pColorTable+12) =0x0000FF00;		// green
	*(pColorTable+13) =0x0000A0FF;		// cyan
	*(pColorTable+14) =0x000000FF;		// blue
	*(pColorTable+15) =0x00D000FF;		// magenta
	// Initialize pixel data
	for (int x=0;x<(ImageWidth+1)/2;x++)
	{
		for (int y=0;y<ImageHeight;y++){
			*(pPixel + y * BytesPerRow + x)  = (uint8)(17 * (16 * y / ImageHeight));
		}
	}
}

uint32 CmBmp4::getWidth()
{
	return ImageWidth;
}

uint32 CmBmp4::getHeight()
{
	return ImageHeight;
}

int32 CmBmp4::getBytesPerRow(int32 nWidth,int32 nBitCount)
{
	return 4 * ((((nWidth * nBitCount + 7) / 8) + 3) / 4);
}

int32 CmBmp4::writeBitmapFile()
{
	// Complete headers and color table
	writeValue((uint8*)mBitmap.getBuffer()+0,  FileType,2);
	writeValue((uint8*)mBitmap.getBuffer()+2,  &FileSize);
	writeValue((uint8*)mBitmap.getBuffer()+6,  &FileReserved);
	writeValue((uint8*)mBitmap.getBuffer()+10, &FileOffBits);
	writeValue((uint8*)mBitmap.getBuffer()+14, &ImageSize);
	writeValue((uint8*)mBitmap.getBuffer()+18, &ImageWidth);
	writeValue((uint8*)mBitmap.getBuffer()+22, &ImageHeight);
	writeValue((uint8*)mBitmap.getBuffer()+26, &ImagePlanes,2);
	writeValue((uint8*)mBitmap.getBuffer()+28, &ImageBitCount,2);
	writeValue((uint8*)mBitmap.getBuffer()+30, &ImageCompression);
	writeValue((uint8*)mBitmap.getBuffer()+34, &ImageSizeImage);
	writeValue((uint8*)mBitmap.getBuffer()+38, &ImageXPixelsPerMeter);
	writeValue((uint8*)mBitmap.getBuffer()+42, &ImageYPixelsPerMeter);
	writeValue((uint8*)mBitmap.getBuffer()+46, &ImageColorsUsed);
	writeValue((uint8*)mBitmap.getBuffer()+50, &ImageColorsImportant);

	// Write bitmap to disk
	mBitmap.writeBinary(FileName.getText());

	return (int32)mBitmap.getLength();
}

uint8* CmBmp4::writeValue(uint8* pValue8,void* pValue,int32 nCount)
{
	for (int i=0;i<nCount;i++){
		*(pValue8 + i) = *(((uint8*)pValue) + i);
	}
	return pValue8;
}

void CmBmp4::setColor(BMP4_COLOR uColor,uint32 uValue)
{
	*(pColorTable+(uint32)uColor) = uValue;
}

uint32 CmBmp4::getColor(BMP4_COLOR uColor)
{
	return *(pColorTable+(uint32)uColor);
}

void CmBmp4::clearBitmap(int32 nColor)
{
	// Initialize pixel data with given value
	for (int x=0;x<(ImageWidth+1)/2;x++)
	{
		for (int y=0;y<ImageHeight;y++){
			*(pPixel + y * BytesPerRow + x)  = (uint8)(17 * (nColor%16));
		}
	}
}

void CmBmp4::drawVerticalRainbowLine(int32 nX1,int32 nY1,int32 nX2,int32 nY2,BMP4_COLOR nStartColor,BMP4_COLOR nEndColor,int32 /*nWidth*/)
{
	if (nStartColor > nEndColor){
		throw CmException( "StartColor is greater than EndColor",
                           static_cast<int32>(nStartColor)       );
	}
	for (int32 nColor=nStartColor;nColor<=nEndColor;nColor++){
		drawLine(nX1,nY1+(nY2-nY1)*(nColor-nStartColor)/(nEndColor-nStartColor+1),nX2,nY1+(nY2-nY1)*(nColor-nStartColor+1)/(nEndColor-nStartColor+1),(BMP4_COLOR)nColor);
	}
}

void CmBmp4::drawLine(int32 nX1,int32 nY1,int32 nX2,int32 nY2,BMP4_COLOR nColor,int32 nWidth)
{
	int32 deltaX;
	int32 deltaY;
	int32 nX;
	int32 nY;
	int32 x;
	int32 y;
	int32 wX;
	int32 wY;
	int32 nMaxY = nY2>nY1 ? nY2:nY1;

	if (nX1 < 0 || nX1 >= ImageWidth){
		throw CmException("Coordinate out of scope X1: ",nX1);
	}
	if (nX2 < 0 || nX2 >= ImageWidth){
		throw CmException("Coordinate out of scope X2: ",nX2);
	}
	if (nY1 < 0 || nY1 >= ImageHeight){
		throw CmException("Coordinate out of scope Y1: ",nY1);
	}
	if (nY2 < 0 || nY2 >= ImageHeight){
		throw CmException("Coordinate out of scope Y2: ",nY2);
	}
	if (nX2 < nX1){
		nX  = nX1;
		nX1 = nX2;
		nX2 = nX;
		nY  = nY1;
		nY1 = nY2;
		nY2 = nY;
	}
	deltaX = nX2 - nX1 + 1;
	deltaY = abs(nY2 - nY1) + 1;
	nX     = deltaX/2;
	nY     = deltaY;

	for (int nX0=nX1, nY0=nY1;nX0<=nX2;nX0++){
		// Plot all Y dots
		do{
			x = nX0;
			y = nY0>nMaxY?nMaxY:nY0;
			y = y<0?0:y;
			// Fill line width
			wX = 0;
			wY = 0;
			for (int w=0;w<nWidth;w++){
				// Plot a single dot
				if (0==x%2){
					*(pPixel + y*BytesPerRow + x/2) &= 0x0F;
					*(pPixel + y*BytesPerRow + x/2) |= (nColor%16)<<4;
				}else{
					*(pPixel + y*BytesPerRow + x/2) &= 0xF0;
					*(pPixel + y*BytesPerRow + x/2) |= nColor%16;
				}
				if (deltaX > deltaY){
					if (wX < wY){
						wX += deltaX;
						x = x>0 ? x-1 : x;
					}else{
						wY += deltaY;
						if (nY2 > nY1){
							y = y<ImageHeight ? y+1 : y;
						}else{
							y = y>0 ? y-1 : y;
						}
					}
				}else{
					if (wX < wY){
						wX += deltaX;
						x = x<ImageWidth ? x+1 : x;
					}else{
						wY += deltaY;
						if (nY2 > nY1){
							y = y>0 ? y-1 : y;
						}else{
							y = y<ImageHeight ? y+1 : y;
						}
					}
				}
			}
			// Check if Y needs to be adjusted
			if (nY >= nX){
				if (nY2 > nY1){
					nY0++;
				}else if (nY2 < nY1){
					nY0--;
				}
				if (0!=deltaX){
					nX += deltaX;
				}else{
					if (0==deltaY){
						break;
					}
				}
			}
		}while(nY >= nX && abs(nY0-nY1) <= abs(nY2-nY1));
		nY += deltaY;
	}
}

//////////////////////////////////////////////////////////////////////
// CmSignalScreen Class
//////////////////////////////////////////////////////////////////////
CmSignalScreen::CmSignalScreen()
:CmBmp4()
{
	// initialize FFT array
	for (int32 nFreq=0; nFreq<2048; nFreq++){
		FFT[nFreq] = 3000 + nFreq;
	}
}

CmSignalScreen::~CmSignalScreen()
{
}

uint32 CmSignalScreen::setAxisX(uint64 u64Left,uint64 u64Period)
{
	u64LeftX   = u64Left;
	u64PeriodX = u64Period;
	// Return necessary shift
	uint32 uWidth = getWidth();
	for (uShiftX=0;uWidth>0;uShiftX++){
		uWidth >>= 1;
	}
	for (;uShiftX>0;uShiftX--){
		u64Left <<= 1;
#ifdef gcc
		if (u64Left & 0x8000000000000000ull){
#else
		if (u64Left & 0x8000000000000000){
#endif
			break;
		}
	}
	return uShiftX;
}

void CmSignalScreen::setZoomX(uint64 u64Zoom,uint64 u64DivisionZoom,uint32 uWidthZoom)
{
	u64ZoomX         = u64Zoom;
	u64DivisionZoomX = u64DivisionZoom;
	uWidthZoomX      = uWidthZoom;
	if (uWidthZoomX > 100){
		throw CmException("WidthZoomX exceeded range 0..100: ",uWidthZoomX);
	}
}

void CmSignalScreen::setShortTermX(uint64 u64ShortTerm,uint64 u64DivisionShortTerm,uint32 uWidthShortTerm)
{
	u64ShortTermX         = u64ShortTerm;
	u64DivisionShortTermX = u64DivisionShortTerm;
	uWidthShortTermX      = uWidthShortTerm;
	if (uWidthZoomX + uWidthShortTermX > 100){
		throw CmException("WidthZoomX+WidthShortTermX exceeded range 0..100: ",uWidthZoomX + uWidthShortTermX);
	}
}

void CmSignalScreen::setLongTermX(uint64 u64LongTerm,uint64 u64DivisionLongTerm,uint32 uWidthLongTerm)
{
	u64LongTermX         = u64LongTerm;
	u64DivisionLongTermX = u64DivisionLongTerm;
	uWidthLongTermX      = uWidthLongTerm;
	if (uWidthZoomX + uWidthShortTermX + uWidthLongTermX > 100){
		throw CmException("WidthZoomX+WidthShortTermX+WidthLongTermX exceeded range 0..100: ",uWidthZoomX + uWidthShortTermX + uWidthLongTermX);
	}
}

uint32 CmSignalScreen::setAxisY(uint64 u64Low,uint64 u64High,uint64 u64Division)
{
	u64LowY		 = u64Low;
	u64HighY	 = u64High;
	u64DivisionY = u64Division==0? u64DivisionY : u64Division;
	// Return necessary shift
	uint32 uWidth = getWidth();
	for (uShiftY=0;uWidth>0;uShiftY++){
		uWidth >>= 1;
	}
	for (;uShiftY>0;uShiftY--){
		u64High >>= 1;
#ifdef gcc
		if (u64High & 0x8000000000000000ull){
#else
		if (u64High & 0x8000000000000000){
#endif
			break;
		}
	}
	return uShiftY;
}

uint64 CmSignalScreen::getZoomX()
{
	return u64ZoomX;
}

uint64 CmSignalScreen::getShortTermX()
{
	return u64ShortTermX;
}

uint64 CmSignalScreen::getLongTermX()
{
	return u64LongTermX;
}

void CmSignalScreen::clearSignalScreen(BMP4_COLOR uColor)
{
	// Clear canvas
	clearBitmap(uColor);
	// Initialize signal screen parameters
	u64LeftX1		= u64LeftX				>> uShiftX;
	u64ZoomX1		= u64ZoomX				>> uShiftX;
	u64ShortTermX1	= u64ShortTermX			>> uShiftX;
	u64LongTermX1	= u64LongTermX			>> uShiftX;
	u64LowY1		= u64LowY				>> uShiftY;
	u64RangeY1		= (u64HighY-u64LowY)	>> uShiftY;
	// Reset begin markers
	for (int i=0;i<16;i++){
		fFirst[i] = true;
	}
	// Align two gray levels for displaying min and max values
	setColor(cGray2,getColor(cGray3));
	setColor(cGray3,getColor(cGray4));
//	setColor(cGray4,getColor(cGray5));
	setColor(cGray4,getColor(cGray6));
	setColor(cGray5,getColor(cGray6));
}

void CmSignalScreen::generateGrid()
{
#define SCREEN_GRID_COLOR_ZOOM		 cGray3
#define SCREEN_GRID_COLOR_SHORT_TERM cGray2
#define SCREEN_GRID_COLOR_LONG_TERM  cGray6
#define SCREEN_GRID_COLOR_HORIZONTAL cGray6
	uint32 uWidth  = getWidth();
	uint32 uHeight = getHeight();
	uint32 uPos;
	uint32 uOffset;
	uint64 u64Pos;
	uint64 u64Step;
	uint64 u64Left;
	// Generate horizontal lines
	u64Step = u64DivisionY >> uShiftY;
	for (u64Pos=u64Step-u64LowY1%u64Step;u64Pos<u64RangeY1;u64Pos+=u64Step){
		uPos = (uint32)(uHeight*u64Pos/u64RangeY1);
		drawLine(0,uPos,uWidth-1,uPos,SCREEN_GRID_COLOR_HORIZONTAL);
	}
	// Generate vertical lines ...
	// ... for Zoom
	//u64Step = u64DivisionZoomX >> uShiftX;
	//u64Left = u64LeftX1;
	//uOffset = 0;
	//for (u64Pos=u64Left-u64Left%u64Step; u64Pos>u64Left-u64ZoomX1; u64Pos-=u64Step){
	//	uPos = (uint32)(uOffset + (uWidth*(u64Left-u64Pos)/u64ZoomX1)*uWidthZoomX/100);
	//	drawLine(uPos,0,uPos,uHeight-1,SCREEN_GRID_COLOR_ZOOM);
	//}
	// ... for rainbow marker line
	//uPos = uWidth*uWidthZoomX/4000;
	//drawVerticalRainbowLine(uPos,0,uPos,uHeight-1,cRed,cMagenta);
	// ... for right blue marker line
	uPos = uWidth*uWidthZoomX/100;
	drawVerticalRainbowLine(uPos,0,uPos,uHeight-1,cBlue,cBlue);
	// ... for ShortTerm
	u64Step = u64DivisionShortTermX >> uShiftX;
	u64Left = u64LeftX1-u64ZoomX1;
	uOffset = uWidth*uWidthZoomX/100;
	for (u64Pos=u64Left-u64Left%u64Step; u64Pos>u64Left-u64ShortTermX1; u64Pos-=u64Step){
		uPos = (uint32)(uOffset + (uWidth*(u64Left-u64Pos)/u64ShortTermX1)*uWidthShortTermX/100);
		drawLine(uPos,0,uPos,uHeight-1,SCREEN_GRID_COLOR_SHORT_TERM);
	}
	// ... for right blue marker line
	uPos = uWidth*(uWidthZoomX+uWidthShortTermX)/100;
	drawVerticalRainbowLine(uPos,0,uPos,uHeight-1,cBlue,cBlue);
	// ... for LongTerm
	u64Step = u64DivisionLongTermX >> uShiftX;
	u64Left = u64LeftX1-u64ZoomX1-u64ShortTermX1;
	uOffset = uWidth*(uWidthZoomX+uWidthShortTermX)/100;
	for (u64Pos=u64Left-u64Left%u64Step; u64Pos>u64Left-u64LongTermX1; u64Pos-=u64Step){
		uPos = (uint32)(uOffset + (uWidth*(u64Left-u64Pos)/u64LongTermX1)*uWidthLongTermX/100);
		drawLine(uPos,0,uPos,uHeight-1,SCREEN_GRID_COLOR_LONG_TERM);
	}
}

void CmSignalScreen::plotSignal(uint64 u64X,double u64Y,BMP4_COLOR uColor,int64 nOffset,uint32 uLineWidth)
{
	uint64 u64Left;
	uint64 u64Range;
	uint32 uOffset;
	uint32 uWidth;

	// Check boundaries
	if (u64X > u64LeftX){
		return;
	}
	if (u64X < u64LeftX-u64PeriodX){
		return;
	}
	if (u64X > u64LeftX-u64ZoomX){
		u64Left  = u64LeftX1;
		u64Range = u64ZoomX1;
		uOffset = 0;
		uWidth  = uWidthZoomX;
	}else if (u64X > u64LeftX-u64ZoomX-u64ShortTermX){
		u64Left  = u64LeftX1-u64ZoomX1;
		u64Range = u64ShortTermX1;
		uOffset = (ImageWidth-1)*uWidthZoomX/100;
		uWidth  = uWidthShortTermX;
	}else if (u64X > u64LeftX-u64ZoomX-u64ShortTermX-u64LongTermX){
		u64Left  = u64LeftX1-u64ZoomX1-u64ShortTermX1;
		u64Range = u64LongTermX1;
		uOffset = (ImageWidth-1)*(uWidthZoomX+uWidthShortTermX)/100;
		uWidth  = uWidthLongTermX;
	}else{
		return;
	}
	if ( (int32)u64Y + nOffset > 0){
		u64Y += nOffset;
	}else{
		u64Y = (double)u64LowY;
	}
	if (u64Y < u64LowY){
		u64Y = (double)u64LowY;
	}
	if (u64Y > u64HighY){
		u64Y = (double)u64HighY;
	}
	uint32 uNewX = (uint32)(uOffset + (ImageWidth-1)*(u64Left-(u64X >> uShiftX))*uWidth/(100*u64Range));
	uint32 uNewY = (uint32)((ImageHeight-1)*((((uint64)u64Y) >> uShiftY)-u64LowY1)/u64RangeY1);
	if (true==fFirst[uColor]){
		uLastX[uColor] = uNewX;
		uLastY[uColor] = uNewY;
		fFirst[uColor] = false;
	}
	drawLine(uLastX[uColor],uLastY[uColor],uNewX,uNewY,uColor,uLineWidth);
	uLastX[uColor] = uNewX;
	uLastY[uColor] = uNewY;
}

void CmSignalScreen::plotFFT(CmString& CmDataXVector)
{
	uint32 uWidthFFT = 100 - uWidthZoomX - uWidthShortTermX - uWidthLongTermX;
	int32  nMaxFFT   = MIN_INT32;
	int32  nMinFFT   = MAX_INT32;
	uint32 uMaxX     = 0;
	uint32 uMinX     = MAX_INT32;
	uint32 uX;
	uint32 uY;
	uint32 uLastY_l = 0;
	uint32 uLastX_l = 0;
	int32  nAvg = 0;
	BMP4_COLOR cColor;
	CmString mBinX;
	CmString CmDataX(1952);

	// Generate FFT background
	for (int32 nX=ImageWidth*(100-uWidthFFT)/100; nX<ImageWidth; nX++){
		drawLine(nX,0,nX,ImageHeight-1,cBlack);
	}
#define FFT_LENGTH 2048
#define FFT_BORDER 48
	// Prepare FFT data
	for (uint32 i=0;i<CmDataXVector.getLength();i++){
		FFT[FFT_BORDER+i] = *(uint8*)((CmDataXVector.getBuffer()+i));
		nAvg += FFT[FFT_BORDER+i];
		if (uMinX > (uint32)FFT[FFT_BORDER+i]){
			uMinX = FFT[FFT_BORDER+i];
		}
		if (uMaxX < (uint32)FFT[FFT_BORDER+i]){
			uMaxX = FFT[FFT_BORDER+i];
		}
	}
	nAvg /= (int32)CmDataXVector.getLength();
	for (int32 i=0;i<FFT_BORDER;i++){
		FFT[i] = nAvg + (FFT[FFT_BORDER]-nAvg)*i/FFT_BORDER;
		FFT[FFT_LENGTH-1-i] = nAvg + (FFT[FFT_LENGTH-1-FFT_BORDER]-nAvg)*i/FFT_BORDER;
	}
	// Write raw data to file
	CmString mBlock;
	CmString mValue;
	for (int32 i=0;i<1024;i++){
		mValue = FFT[2*i]+FFT[2*i+1] - 2*nAvg;
		mBlock += mValue;
		mBlock += "\n";
	}
	mBlock.writeBinary("block.csv");
	// Calculate FFT
#ifdef _CALCULATE_FFT
	for (int32 i=0;i<1024;i++){
		fft[i].re  = FFT[2*i]+FFT[2*i+1] - 2*nAvg;
		fft[i].im  = 0;
	}
	fftinit();
	fft1024(fft);
	for (int32 i=0;i<1024;i++){
		FFT[2*i]  = (uint32)sqrt(fft[i].re*fft[i].re + fft[i].im*fft[i].im);
		FFT[2*i+1] = FFT[2*i];
	}
#endif
	// Reset DC component
	for (int32 i=0;i<10;i++){
		FFT[i] = 0; 
	} 
	// Determine min and max FFT value
	for (int32 nFreq=0; nFreq<2048; nFreq++){
		if (FFT[nFreq] < nMinFFT){
			nMinFFT = FFT[nFreq];
		}
		if (FFT[nFreq] > nMaxFFT){
			nMaxFFT = FFT[nFreq];
		}
	}
	// Plot data values
#define FFT_DATA_OFFSET 60
	for (uint32 i=0;i<CmDataXVector.getLength();i++){
		uX = *(uint8*)(CmDataXVector.getBuffer()+i);
		uX = ImageWidth-1-(ImageWidth*uWidthFFT*FFT_DATA_OFFSET/9700 + (ImageWidth*uWidthFFT*(94-FFT_DATA_OFFSET)*(uX-uMinX))/((uMaxX-uMinX)*10000));
		uY = (ImageHeight-1)*i/(int32)CmDataXVector.getLength();
		if (0==i){
			uLastX_l = uX;
			uLastY_l = uY;
		}
		drawLine(uLastX_l,uLastY_l,uX,uY,cGray3);
		uLastX_l = uX;
		uLastY_l = uY;
	}
	// Plot FFT values
	for (int32 nFreq=0; nFreq<2048; nFreq++){
		uX     = ImageWidth-1-ImageWidth*(FFT[nFreq]-nMinFFT)*uWidthFFT/((nMaxFFT-nMinFFT+1)*10000/FFT_DATA_OFFSET);
		uY     = (ImageHeight-1)*nFreq/2048;
		cColor = (BMP4_COLOR)(cRed + (cMagenta-cRed+1)*nFreq/2048);
		drawLine(uX,uY,ImageWidth-1,uY,cColor);
	}
	// Generate grid
#define FFT_GRID 5
	for (int32 nX=ImageWidth*(100-(FFT_GRID)*uWidthFFT/FFT_GRID)/100; nX<ImageWidth; nX+=ImageWidth*(uWidthFFT/FFT_GRID)/100){
		drawLine(nX,0,nX,ImageHeight-1,cGray6);
	}
}


#ifdef TO_BE_DONE
//////////////////////////////////////////////////////////////////////
// MPos Class
//////////////////////////////////////////////////////////////////////
MPos::MPos()
{
	// Set initial distance to 1m
	uData = 1;
}

MPos::~MPos(){}

//Access functions
MTime* MPos::setTime(MTime* pTime){return (MTime*)setBack((MNode*)pTime);}
MTime* MPos::getTime(){return (MTime*)getBack();}
double MPos::getDistance(){return (double)(((unsigned)getNext()/0x100000000)+(unsigned)getData());} 
double MPos::getLongitude(){return (unsigned)getMore()/10000000;}
double MPos::getLatitude(){return (long)getLess()/10000000;}
//Mark pointers which are used as values
bool MPos::isLessPtr(){return false;};
bool MPos::isMorePtr(){return false;};
bool MPos::isNextPtr(){return false;};
bool MPos::isDataPtr(){return false;};
//Report special features and runtime type 
bool MPos::isAttachement(){return true;};
MNodeType MPos::getNodeType(){return M_POS;};


//////////////////////////////////////////////////////////////////////
// MView Class
//////////////////////////////////////////////////////////////////////
MView::MView()
{
	uData = 450000000;
}

MView::~MView(){}

//Access functions
MPos* MView::setPos(MPos* pPos){return (MPos*)setBack(pPos);}
MPos* MView::getPos(){return (MPos*)getBack();}
double MView::getAzimuth(){return  ((unsigned)getMore())/10000000;}
double MView::getAltitude(){return ((int)getLess())/10000000;}	
double MView::getAperture(){return ((unsigned)getData())/10000000;}
double MView::getRotation(){return ((int)getNext())/10000000;}
unsigned MView::getIncAngle(){return getData()/200;}
//Mark pointers which are used as values
bool MView::isLessPtr(){return false;};
bool MView::isMorePtr(){return false;};
bool MView::isNextPtr(){return false;};
bool MView::isDataPtr(){return false;};
//Report special features and runtime type 
bool MView::isAttachement(){return true;};
MNodeType MView::getNodeType(){return M_VIEW;};


//////////////////////////////////////////////////////////////////////
// MScreen Class
//////////////////////////////////////////////////////////////////////
MScreen::MScreen(){}
MScreen::~MScreen(){}

//Access functions
MView* MScreen::setView(MView* pView){return (MView*)setBack(pView);}
MView* MScreen::getView(){return (MView*)getBack();}
//Mark pointers which are used as values
bool MScreen::isLessPtr(){return false;};
bool MScreen::isMorePtr(){return false;};
bool MScreen::isNextPtr(){return false;};
bool MScreen::isDataPtr(){return false;};
//Report special features and runtime type 
bool MScreen::isAttachement(){return true;};
MNodeType MScreen::getNodeType(){return M_SCREEN;};

void MScreen::setPointer(unsigned _uX, unsigned _uY)
{
	uData = (_uX & 0x0000FFFF) | ((_uY << 16) & 0xFFFF0000);
}

void MScreen::setScreenSize(unsigned _uWidth, unsigned _uHeight)
{
	pNext = (MNode*)((_uWidth & 0x0000FFFF) | ((_uHeight << 16) & 0xFFFF0000));
}

unsigned MScreen::getPointerX()
{
	return uData & 0x0000FFFF;
}

unsigned MScreen::getPointerY()
{
	return (uData >> 16) & 0x0000FFFF;
}

unsigned MScreen::getScreenWidth()
{
	return (unsigned)pNext & 0x0000FFFF;
}

unsigned MScreen::getScreenHeight()
{
	return ((unsigned)pNext >> 16) & 0x0000FFFF;
}


//////////////////////////////////////////////////////////////////////
// MColor Class
//////////////////////////////////////////////////////////////////////
MColor::MColor(){};
MColor::~MColor(){};

//Access functions
void MColor::setColor(unsigned uColor){setData(uColor);}
unsigned MColor::getColor(){return getData();}

//Report special features and runtime type 
bool MColor::isAttachement(){return true;};
MNodeType MColor::getNodeType(){return M_COLOR;};



//////////////////////////////////////////////////////////////////////
// MDisplay Class
//////////////////////////////////////////////////////////////////////
MDisplay::MDisplay()
{
	uWidth  = 0;
	uHeight = 0;
	uScan   = 0;
	pPixel  = NULL;
}
MDisplay::~MDisplay()
{
	if(NULL!=pPixel){
		delete[] pPixel;
	}
}

void MDisplay::setDisplaySize(unsigned _uWidth,unsigned _uHeight)
{
	// Remove current pixel array and create a new one
	if(NULL!=pPixel){
		delete[] pPixel;
	}
	uWidth  = _uWidth;
	uHeight = _uHeight;
	uScan   = (sizeof(MPixel)*uWidth+3) & 0xFFFFFFFC; 
	pPixel = new BYTE[uScan*uHeight];
}

unsigned MDisplay::getWidth()
{
	return uWidth;
}

unsigned MDisplay::getHeigth()
{
	return uHeight;
}

void MDisplay::setPixel(unsigned uX,unsigned uY,MPixel mPixel)
{
	// Check pixel array's boundaries and assign a new pixel value
	if(uX < uWidth && uY < uHeight){
		*(MPixel*)(pPixel + uScan*uY + sizeof(MPixel)*uX) = mPixel;
	}else{
		throw(CmException("Display boundaries exceeded"));
	}
}

void MDisplay::setFluxPixel(unsigned uX,unsigned uY,unsigned uFluxPixel)
{
	// Check pixel array's boundaries and assign a new pixel value
	if(uX < uWidth && uY < uHeight){
		*(unsigned*)(pPixel + uScan*uY + sizeof(MPixel)*uX) =
				 (uFluxPixel&0x0000007F) << 17	// red
				|(uFluxPixel&0x00003F80) << 2	// green
				|(uFluxPixel&0x000FC000) >> 12;	// blue
	}else{
		throw(CmException("Display boundaries exceeded"));
	}
}

MPixel MDisplay::getPixel(unsigned uX,unsigned uY)
{
	// Check pixel array's boundaries and return requested pixel value
	if(uX < uWidth && uY < uHeight){
		return *(MPixel*)(pPixel + uScan*uY + sizeof(MPixel)*uX);
	}else{
		throw(CmException("Display boundaries exceeded"));
	}
}

void MDisplay::initSpectrum()
{
	// Init display with a nice looking color spectrum
	MPixel mPixel;
	for(unsigned uX=0;uX<uWidth;uX++){
		for(unsigned uY=0;uY<uHeight;uY++){
			mPixel.Red   = 256 * uY/uHeight;
			mPixel.Green = 256 * (uWidth-uX-1)/uWidth;
			mPixel.Blue  = 256 * (uX+uY)/(uWidth+uHeight);
			setPixel(uX,uY,mPixel);
		}
	}
}

#endif
