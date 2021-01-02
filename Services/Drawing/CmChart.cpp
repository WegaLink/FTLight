//////////////////////////////////////////////////////////////////////////////
//
// CmChart.cpp: Implementation of the CmChart classes
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

#define Chart_Config
#include "CmChart.h"

/*------CmChart---------------------ToDo--------------------------------------
*
*
*/

//------Constructor-and-destructor--------------------------------------------
CmChart::CmChart(const int8 *_Init)
	:CmValueINI(_Init)
{
	// initialize workspace
	TextCount = 0;
	LockGUI = NULL;
	isDraw = NULL;
	Pro = NULL;
	Dyn = NULL;

	// initialize default CmValueINI arrays
	CmValueINI::setDefaultInfoFTL(pro_.UURI, dyn_.LogLevel);
	CmValueINI::setDefaultInfoFTL(dyn_.UURI, dyn_.LogLevel);

}
CmChart::~CmChart()
{

}

//------Layout----------------------------------------------------------------

bool CmChart::clearChart(CmString _Title)
{
	LockGUI != NULL ? LockGUI->enterSerialize() : 0;

	// stop drawing
	dyn().Init.setDrawingEnabled(false);

	// background
	dyn().Init.setChartBackground(pro().BackgroundColor, pro().BackgroundAlpha);

	// title
	TextCount = 0;
	dyn().Text.getMatrix().clearMatrix();
	CmPoint2D TitlePos(float(pro().TitleLeft), float(dyn().Height) - float(pro().TitleTop));
	dyn().Text.setChartText(TextCount, TitlePos, _Title, pro().TitleFontSize, -1, pro().TitleFontColor, pro().TitleFontAlpha, pro().TitleFont);

	// clear grid
	dyn().LayoutGrid.getMatrix().clearMatrix();
	dyn().LayoutLabels.getMatrix().clearMatrix();

	// clear signals
	dyn().SignalPoints.getMatrix().clearMatrix();
	dyn().SignalTags.getMatrix().clearMatrix();

	// enable drawing again
	dyn().Init.setDrawingEnabled(true);

	LockGUI != NULL ? LockGUI->leaveSerialize() : 0;

	return true;
}
bool CmChart::updateChart()
{

	if (isDraw != NULL){
		LockGUI != NULL ? LockGUI->enterSerialize() : 0;
		*isDraw = true;
		LockGUI != NULL ? LockGUI->leaveSerialize() : 0;
	}

	return true;
}
bool CmChart::drawLayout(bool _isChartOff)
{
	LockGUI != NULL ? LockGUI->enterSerialize() : 0;

	// stop drawing
	dyn().Init.setDrawingEnabled(false);

	// background
	dyn().Init.setChartBackground(pro().BackgroundColor, pro().BackgroundAlpha);

	// title
	TextCount = 0;
	dyn().Text.getMatrix().clearMatrix();
	CmPoint2D TitlePos(float(pro().TitleLeft), float(dyn().Height) - float(pro().TitleTop));
	dyn().Text.setChartText(TextCount, TitlePos, pro().Title, pro().TitleFontSize, -1, pro().TitleFontColor, pro().TitleFontAlpha, pro().TitleFont);

	// chart
	LineCount = 0;
	if (_isChartOff){
		// chart is off, thus draw only a transparent chart background
		const double Transparency = 0.3;
		CmRectangle2D Canvas(CmPoint2D(float(pro().CanvasLeft), float(dyn().Height) - float(pro().CanvasTop)), CmPoint2D(float(dyn().Width) - float(pro().CanvasRight), float(pro().CanvasBottom)));
		dyn().LayoutGrid.setChartRectangle(LineCount, Canvas, pro().CanvasBorderWidth, pro().CanvasBorderColor, pro().CanvasFillColor, pro().CanvasBorderAlpha, Transparency);
		// clear signals
		dyn().SignalPoints.getMatrix().clearMatrix();
		dyn().SignalTags.getMatrix().clearMatrix();
	}
	else{
		// canvas
		CmRectangle2D Canvas(CmPoint2D(float(pro().CanvasLeft), float(dyn().Height) - float(pro().CanvasTop)), CmPoint2D(float(dyn().Width) - float(pro().CanvasRight), float(pro().CanvasBottom)));
		dyn().LayoutGrid.setChartRectangle(LineCount, Canvas, pro().CanvasBorderWidth, pro().CanvasBorderColor, pro().CanvasFillColor, pro().CanvasBorderAlpha, pro().CanvasFillAlpha);
	}

	// enable drawing again
	dyn().Init.setDrawingEnabled(true);

	LockGUI != NULL ? LockGUI->leaveSerialize() : 0;

	return true;
}

bool CmChart::addText(CmString _Text)
{
	LockGUI != NULL ? LockGUI->enterSerialize() : 0;
	
	// stop drawing
	dyn().Init.setDrawingEnabled(false);

	CmPoint2D TextPos(float(pro().TextLeft), float(dyn().Height) - float(pro().TextTop) - TextCount * float(pro().TextSpace));
	if (false == dyn().Text.setChartText(TextCount, TextPos, _Text, pro().TextFontSize, -1, pro().TextFontColor, pro().TextFontAlpha, pro().TextFont)){ LockGUI->leaveSerialize(); return false; }

	// enable drawing again
	dyn().Init.setDrawingEnabled(true);

	LockGUI != NULL ? LockGUI->leaveSerialize() : 0;

	return true;
}

bool CmChart::drawSignal(CmSignalType _SignalType)
{
	LockGUI != NULL ? LockGUI->enterSerialize() : 0;
	
	// test signal
	if (SIGNAL_Curve != _SignalType){
		// clear chart
		dyn().SignalPoints.getMatrix().clearMatrix();
		// baseline
		pro().SignalType = SIGNAL_Baseline;
		if (false == drawSignalTest()){ LockGUI->leaveSerialize(); return false; }
		// signal line
		pro().SignalType = int32(_SignalType);
		if (false == drawSignalTest()){ LockGUI->leaveSerialize(); return false; }
	}

	LockGUI != NULL ? LockGUI->leaveSerialize() : 0;

	return true;
}

bool CmChart::drawSignalTest()
{
	LockGUI != NULL ? LockGUI->enterSerialize() : 0;

	// get signal params
	CmSignalType SignalType = CmSignalType(int32((pro().SignalType)));
	float ChartOffset = float(pro().CanvasLeft) + 1;
	float Points = float(dyn().Width) - float(pro().CanvasRight) - ChartOffset + 1;
	float Amplitude = (float(dyn().Height) - float(pro().CanvasTop) - float(pro().CanvasBottom) - float(pro().SignalLineWidth)) / 2;
	float Baseline = float(pro().CanvasBottom) + Amplitude + float(pro().SignalLineWidth) / 4;

	// access to signal points
	CmMatrix& M = dyn().SignalPoints.getMatrix();

	// stop drawing
	dyn().Init.setDrawingEnabled(false);

	// get index
	int32 Index = M(0).getSizeLastLevel();

	// drawing params
	M(1, Index) = float(SIGNAL_Baseline == SignalType ? pro().SignalBaselineWidth : pro().SignalLineWidth);
	M(2, Index) = CmString(SIGNAL_Baseline == SignalType ? pro().SignalBaselineColor : pro().SignalLineColor);
	M(4, Index) = double(SIGNAL_Baseline == SignalType ? pro().SignalBaselineAlpha : pro().SignalLineAlpha);

	// get phase
	double Phase = pro().SignalPhase;
	int32 PhaseOffset = int32(Points * (Phase - 2 * PI * floor(Phase / (2 * PI))) / (2 * PI));

	// generate signal points
	for (int32 i = 0; i < Points; i++){
		int32 p = (PhaseOffset + i) % int32(Points);
		float Y = Baseline;
		switch (SignalType){
		case SIGNAL_Baseline:  break;
		case SIGNAL_Sine:  Y += float(Amplitude * sin(2 * PI * p / Points)); break;
		case SIGNAL_Square: Y += p < Points / 2 ? Amplitude : -Amplitude; break;
		case SIGNAL_Triangle: Y += Amplitude * 4 * (p < Points / 4 ? p : p < Points * 3 / 4 ? Points / 2 - p : p - Points) / Points  ; break;
		case SIGNAL_SawtoothUp: Y += Amplitude * (p < Points / 2 ? 2 * p : 2 * (p - Points)) / Points; break;
		case SIGNAL_SawtoothDown: Y -= Amplitude * (p < Points / 2 ? 2 * p : 2 * (p - Points)) / Points; break;

		default: break;
		}
		M(0, Index, 0, i) = ChartOffset + i;
		M(0, Index, 1, i) = Y;
	}

	// switch phase forward
	const double PhaseIncrement = 2 * PI / Points;
	SIGNAL_Baseline != SignalType ? pro().SignalPhase = Phase + PhaseIncrement : 0;

	// enable drawing signal points
	M() = 1;

	// enable drawing again
	dyn().Init.setDrawingEnabled(true);

	LockGUI != NULL ? LockGUI->leaveSerialize() : 0;

	return true;
}

bool CmChart::drawSignalTest(double _Phase1, double _Phase2)
{
	LockGUI != NULL ? LockGUI->enterSerialize() : 0;
	
	// background
	bool Return = dyn().Init.setChartBackground(pro().BackgroundColor, pro().BackgroundAlpha);

	// position
	float Top = dyn().Height;
	const float TopFirst = pro().TextTop;
	const float Left = pro().TextLeft;
	const float Space = pro().TextSpace;

	// set text defaults
	const double Rotation = 0.0;
	dyn().Text.clearMatrix();
	dyn().Text.setChartTextDefaults(pro().TextFontSize, Rotation, pro().TextFontColor, pro().TextFontAlpha);

	// generate phase information
	CmString PhaseLocal("    Local phase: ");
	CmString PhaseRemote("Remote phase: ");
	CmString Phase;
	// local
	Phase.double2String(_Phase1, 0);
	PhaseLocal += Phase;
	PhaseLocal += "°";
	// remote
	Phase.double2String(_Phase2, 0);
	PhaseRemote += Phase;
	PhaseRemote += "°";

	// display phase information
	int32 TextCount = 0;
	Return == true ? Return = dyn().Text.setChartText(TextCount, CmPoint2D(Left, Top -= TopFirst), PhaseLocal) : 0;
	Return == true ? Return = dyn().Text.setChartText(TextCount, CmPoint2D(Left, Top -= Space), PhaseRemote) : 0;

	// enable drawing again
	dyn().Init.setDrawingEnabled(true);

	LockGUI != NULL ? LockGUI->leaveSerialize() : 0;

	return Return;
}

double CmChart::getSignalPhaseOffsetDeg()
{
	// estimate signal phase offset in the base range 0..360°
	double Phase = pro().SignalPhase;
	return 360 * (Phase - 2 * PI * floor(Phase / (2 * PI))) / (2 * PI);
}

//------CONFIGURATION-MANAGEMENT-functions------------------------------------

// GUI synchronization
bool CmChart::setLockGUI(CmParallelFTL* _LockGUI, bool* _isDraw)
{
	LockGUI = _LockGUI;
	isDraw = _isDraw;
	return NULL == LockGUI ? false : true;
}

// Profile
ChartProfile& CmChart::pro()
{
	if (NULL == Pro){
		// create a local profile from default config
		Pro = new ChartProfile(pro_);
		// fallback to default profile if 'new' failed
		NULL == Pro ? Pro = &pro_ : 0;	
	}

	return *Pro;
}
// Dynamic
ChartDynamic& CmChart::dyn()
{
	if (NULL == Dyn){
		// create a local dynamic from default config
		Dyn = new ChartDynamic(dyn_);
		// fallback to default dynamic if 'new' failed
		NULL == Dyn ? Dyn = &dyn_ : 0;
	}

	return *Dyn;
}

