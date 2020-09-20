//////////////////////////////////////////////////////////////////////////////
//
// CmChart.h: Declaration of CmChart classes
//
//////////////////////////////////////////////////////////////////////////////
//
// author: 	   Eckhard Kantz
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


#ifndef CmChartH
#define CmChartH

#include "FTLight/CmStringFTL.h"

#define CHART_VERSION					"v1.0"

namespace Cosmos
{

typedef struct _ChartProfile ChartProfile;
typedef struct _ChartDynamic ChartDynamic;

typedef enum {
	SIGNAL_Curve			= 0,
	SIGNAL_Baseline			= 1,
	SIGNAL_Sine				= 2,
	SIGNAL_Square			= 3,
	SIGNAL_Triangle			= 4,
	SIGNAL_SawtoothUp		= 5,
	SIGNAL_SawtoothDown		= 6,
} CmSignalType;

/**
 *  CmChart.
 *  The CmChart class provides for the functionality to generate a chart layout
 *  as well as signal curves that both are supposed to be drawn on screen
 */
class CmChart : public CmValueINI
{
	// UURI extensions for a 'CmChart' module
#define UURI_Chart	"EKD@JN58nc_Türkenfeld.CmChart/"CHART_VERSION

public:
	//------Constructor-and-destructor------------------------------------------
	CmChart(const int8 *_Init);
  ~CmChart();

	//------Layout--------------------------------------------------------------

public:
	// chart background, grid and axes will be drawn
	bool drawLayout(bool _isChartOff);

public:
	// a signal curve will be drawn
	bool drawSignal(CmSignalType _SignalType);

public:
	// a signal curve will be drawn
	bool drawSignalTest();

public:
	// signal phase offset [deg]
	double getSignalPhaseOffsetDeg();

	//------CONFIGURATION-MANAGEMENT-functions----------------------------------

public:
	// Profile
	ChartProfile& pro();

public:
	// Dynamic
	ChartDynamic& dyn();


	//--------------workspace---------------------------------------------------

private:
	// config
	ChartProfile* Pro;
	ChartDynamic* Dyn;

};

//============================================================================
// Chart_Config
//============================================================================
#pragma warning (disable : 4510 4610)

//------Chart-profile---------------------------------------------------------

struct _ChartProfile {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	// background 
	CmValueINI Background;
	CmValueINI BackgroundColor;
	CmValueINI BackgroundAlpha;
	// title
	CmValueINI Title;
	CmValueINI TitleTop;
	CmValueINI TitleLeft;
	CmValueINI TitleFont;
	CmValueINI TitleFontSize;
	CmValueINI TitleFontColor;
	CmValueINI TitleFontAlpha;
	// canvas
	CmValueINI Canvas;
	CmValueINI CanvasTop;
	CmValueINI CanvasBottom;
	CmValueINI CanvasLeft;
	CmValueINI CanvasRight;
	CmValueINI CanvasBorderWidth;
	CmValueINI CanvasBorderColor;
	CmValueINI CanvasBorderAlpha;
	CmValueINI CanvasFillColor;
	CmValueINI CanvasFillAlpha;
	// signal
	CmValueINI SignalType;
	CmValueINI SignalPhase;
	CmValueINI SignalLine;
	CmValueINI SignalLineWidth;
	CmValueINI SignalLineColor;
	CmValueINI SignalLineAlpha;
	CmValueINI SignalBaseline;
	CmValueINI SignalBaselineWidth;
	CmValueINI SignalBaselineColor;
	CmValueINI SignalBaselineAlpha;

};

#ifdef Chart_Config
// initialize profile
ChartProfile pro_ =
{
	// identification
	UURI_Chart,
	"/Timestamp",
	// window
	"/Background:window settings",
	"Color:DarkSeaGreen",
	"Alpha:1.0",
	// title
	"/Title:Local Chart Title",
	"Top:5",
	"Left:10",
	"Font:Arial",
	"FontSize:12",
	"FontColor:Black",
	"FontAlpha:1.0",
	// canvas
	"/Canvas:drawing area",
	"Top:30",
	"Bottom:25",
	"Left:25",
	"Right:10",
	"BorderWidth:1",
	"BorderColor:Black",
	"BorderAlpha:1",
	"FillColor:White",
	"FillAlpha:1",
	// signal
	"/Signal:type",
	">Phase:0",
	"Line",
	">Width:2",
	"Color:Black",
	"Alpha:1",
	"<Baseline",
	">Width:1",
	"Color:Black",
	"Alpha:1",


};
#endif

//------Chart-dynamic---------------------------------------------------------

struct _ChartDynamic {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	CmValueINI LogLevel;
	CmValueINI Message;
	CmValueINI Context;
	// chart  
	CmValueINI Chart;
	CmValueINI Width;
	CmValueINI Height;
	CmValueINI Init;
	CmValueINI Text;
	CmValueINI Layout;
	CmValueINI LayoutGrid;
	CmValueINI LayoutLabels;
	CmValueINI Signal;
	CmValueINI SignalPoints;
	CmValueINI SignalTags;

};

#ifdef Chart_Config
// initialize dynamics
ChartDynamic dyn_ =
{
	// identification
	UURI_Chart,
	"/timestamp",
	">loglevel",
	"message",
	"context",
	// chart
	"/Chart:dynamic data",
	">Width:480",
	"Height:270",
	"Init:general canvas settings",		// [`] = enabled, [0] = background, [1] = alpha, [2] = Xmin, [3] = Xmax, [4] = Ymin, [5] = Ymax
	"Text:array of T text items",			// [`] = enabled, [i] = default, [0, t] = disabled, [0, t, 0 / 1] = position x / y, [1, t] = text, [2, t] = size, [3, t] = rotation, [4, t] = color, [5, t] = alpha, [6, t] = font
	"Layout:static chart layout",
	">Grid:array of N lines that form a chart grid",	// [`] = enabled, [i] = default, [0, n] = disabled, [0, n, 0, 0.. / 1, 0..] = start / end, [1, n] = width, [2, n] = color, [3, n] = alpha
	"Labels:array of L text labels for chart axis",	// [`] = enabled, [i] = default, [0, l] = disabled, [0, l, 0 / 1] = position x / y, [1, l] = text, [2, l] = size, [3, l] = rotation, [4, l] = color, [5, l] = alpha, [6, l] = font
	"<Signal:dynamic chart content",
	">Points:array of P polygons for drawing signal curves",	// [`] = enabled, [0, p] = disabled, [0, p, 0, 0.. / 1, 0..] = points x/y, [i] = default, [1, p] = width, [2, p] = line color, [3, p] = fill color, [4, p] = line alpha, [5, p] = fill alpha
	"Tags:array of T text items",			// [`] = enabled, [i] = default, [0, t] = disabled, [0, t, 0 / 1] = position x / y, [1, t] = text, [2, t] = size, [3, t] = rotation, [4, t] = color, [5, t] = alpha, [6, t] = font

};
#endif

}  // namespace Cosmos

using namespace Cosmos;

#endif
