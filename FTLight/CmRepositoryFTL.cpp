//////////////////////////////////////////////////////////////////////////////
//
// CmRepositoryFTL.cpp: Implementation of the CmRepositoryFTL classes
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

#define CmRepositoryFTL_InfoX

#include "FTLight/CmRepositoryFTL.h"

using namespace Cosmos;

//-----------------------------------------------------------------------------
// SERVICE_CmRepositoryFTL
//-----------------------------------------------------------------------------
//
SERVICE_CmRepositoryFTL::SERVICE_CmRepositoryFTL()
    // Initialize SERVICE UURI (=interface) for a 'Cosmos' root UURI
    : CmPlugNode(UURI_SERVICE_CmRepositoryFTL)
{
	// Initialize
  LocalProvider = NULL;
}

SERVICE_CmRepositoryFTL::~SERVICE_CmRepositoryFTL()
{
  // Delete a local SERVICE implementation (PROVIDER) if it exists
  if( NULL!=LocalProvider )
    delete LocalProvider;
}

//------PlugNode-connectivity--------------------------------------------------

CmUURI& SERVICE_CmRepositoryFTL::getUURI()
{
	return Provider().getUURI();
}

bool SERVICE_CmRepositoryFTL::processInformation(CmString& _Information)
{
	return Provider().processInformation(_Information);
}

//bool SERVICE_CmRepositoryFTL::setViewer(SERVICE_CmHexagonViewer *_Viewer, int32 _ViewerSlot)
//{
//	return Provider().setViewer(_Viewer, _ViewerSlot);
//}
//
//int32 SERVICE_CmRepositoryFTL::getViewerSlot()
//{
//	return Provider().getViewerSlot();
//}
//
//SERVICE_CmHexagonViewer * SERVICE_CmRepositoryFTL::getViewer()
//{
//	return Provider().getViewer();
//}

//------RepositoryX-operation--------------------------------------------------

bool SERVICE_CmRepositoryFTL::testChart()
{
	return Provider().testChart();
}

bool SERVICE_CmRepositoryFTL::displayChart()
{
	return Provider().displayChart();
}


//------Service-access-by-PROVIDER---------------------------------------------

PROVIDER_CmRepositoryFTL& SERVICE_CmRepositoryFTL::Provider()
{
  // Instantiate a PROVIDER class if it does not exist yet
  if( NULL==ServiceProvider )
  {
    LocalProvider = new PROVIDER_CmRepositoryFTL();
    ServiceProvider = LocalProvider;
  }

  // Throw exception in case of memory problems
  if( NULL==ServiceProvider )
    throw(0);

  return *(PROVIDER_CmRepositoryFTL*)ServiceProvider;
}


//-----------------------------------------------------------------------------
// PROVIDER_CmRepositoryFTL
//-----------------------------------------------------------------------------
//
PROVIDER_CmRepositoryFTL::PROVIDER_CmRepositoryFTL()
{
  // Extend module's SERVICE UURI with a PROVIDER UURI
  ServiceUURI += UURI_PROVIDER_CmRepositoryFTL;

  // initialize workspace
  //Viewer = NULL;

}

PROVIDER_CmRepositoryFTL::~PROVIDER_CmRepositoryFTL()
{

}

//------PlugNode-connectivity-------------------------------------PROVIDER-----

CmUURI& PROVIDER_CmRepositoryFTL::getUURI()
{
  // Actual PROVIDER implementation of the 'getUURI()' SERVICE function
  return ServiceUURI;
}

bool PROVIDER_CmRepositoryFTL::processInformation(CmString& _Information)
{
	// insert/overwrite Information in local store
	if (false == InfoX.processStringFTL(_Information)) return false;

	// get control string
	CmString Control;
	//if (false == InfoX.getValue(CmRepositoryFTL_COMMAND,Control)) return false;
	if (false == ReX.Command.updateFrom(InfoX)) return false;

	// test chart
	if (ReX.Command == "test") {
		return testChart();
	}

	// display chart
	if (ReX.Command == "display") {
		return displayChart();
	}


	// display received information
	//InfoX.displayCmStringFTL(InfoX, *Viewer, CmPoint2D(200, 100));

	return true;
}

//bool PROVIDER_CmRepositoryFTL::setViewer(SERVICE_CmHexagonViewer *_Viewer, int32 _ViewerSlot)
//{
//	Viewer = _Viewer;
//	ViewerSlot = _ViewerSlot;
//
//	return true;
//}
//
//SERVICE_CmHexagonViewer * PROVIDER_CmRepositoryFTL::getViewer()
//{
//	return Viewer;
//}
//
//int32 PROVIDER_CmRepositoryFTL::getViewerSlot()
//{
//	return ViewerSlot;
//}

//------RepositoryX-operation-------------------------------------PROVIDER-----

bool PROVIDER_CmRepositoryFTL::testChart()
{
 // // check for valid viewer interface
 // if (NULL == Viewer) return false;

	//// determine range
 // CmRange AxisX(-1500, 1500);
 // CmRange AxisY(0, 1000);

	//// bounding rectangle
	//CmPoint2D UpperLeft;
	//CmPoint2D BottomRight;

	//Viewer->generateChartAxes(CmString("RepositoryX"), AxisX, AxisY, UpperLeft, BottomRight);

	//// draw a sine curve as polygon
	//const float PI = 3.1415f;
	//const float dPhi = PI / 60.0f;
	//int32 NumPointsX = (int32)(BottomRight.x() - UpperLeft.x() + 1);
	//int32 RangeY = (int32)(BottomRight.y() - UpperLeft.y() - 2);
	//CmPoint2D *Points = new CmPoint2D[NumPointsX];

	//for (int32 n = 0; n < 60; n++){
	//	for (int32 x = 0; x < NumPointsX; x++){
	//		Points[x].x() = UpperLeft.x() + x;
	//		Points[x].y() = (float)(BottomRight.y() - RangeY * (1 + sin(2 * PI * x / NumPointsX + n * dPhi)) / 2 - 1);
	//	}
	//	Sleep(40);
	//	// clear chart
	//	Viewer->clearChart();
	//	// polygon
	//	float LineWidth = 2;
	//	float LineAlpha = 1.0;
	//	float FillAlpha = 0.0;
	//	CmString LineColor("Red");
	//	CmString FillColor("White");
	//	Viewer->displayPolygonExt(Points, NumPointsX, LineWidth, LineColor, LineAlpha, FillColor, FillAlpha);
	//}

	//delete[] Points;

  return true;
}


bool PROVIDER_CmRepositoryFTL::displayChart()
{
	//// check viewer availability
	//if (NULL == Viewer) return false;

	//// update parameters
	//// chart
	//if (false == ReX.ChartTitle.updateFrom(InfoX)) return false;
	//// stream
	//if (false == ReX.FTLightUURI.updateFrom(InfoX)) return false;
	//if (false == ReX.FTLightRoot.updateFrom(InfoX)) return false;
	//if (false == ReX.FileName.updateFrom(InfoX)) return false;
	//if (false == ReX.FilePeriod.updateFrom(InfoX)) return false;
	//// values
	//if (false == ReX.ValueSamples.updateFrom(InfoX)) return false;
	//if (false == ReX.ValueColumns.updateFrom(InfoX)) return false;
	//if (false == ReX.ValueIndexFirst.updateFrom(InfoX)) ReX.ValueIndexFirst.setValue(0);
	//if (false == ReX.ValueIndexMin.updateFrom(InfoX)) ReX.ValueIndexMin.setValue(1);
	//if (false == ReX.ValueIndexMax.updateFrom(InfoX)) ReX.ValueIndexMax.setValue(2);
	//if (false == ReX.ValueIndexTotal.updateFrom(InfoX)) ReX.ValueIndexTotal.setValue(3);
	//if (false == ReX.ValueIndexTravel.updateFrom(InfoX)) ReX.ValueIndexTravel.setValue(4);
	//// time
	//if (false == ReX.TimeStart.updateFrom(InfoX)) return false;
	//if (false == ReX.TimeEnd.updateFrom(InfoX)) return false;
	//if (false == ReX.TimeLabel.updateFrom(InfoX)) return false;
	//if (false == ReX.TimeDiRemote.updateFrom(InfoX)) return false;
	//// signal
	//if (false == ReX.SignalLabel.updateFrom(InfoX)) return false;
	//if (false == ReX.SignalScale.updateFrom(InfoX)) return false;
	//if (false == ReX.SignalOffset.updateFrom(InfoX)) return false;
	//if (false == ReX.SignalMax.updateFrom(InfoX)) return false;
	//if (false == ReX.SignalMin.updateFrom(InfoX)) return false;
	//// travel
	//if (false == ReX.TravelBalance.updateFrom(InfoX)) return false;
	//if (false == ReX.TravelFactor.updateFrom(InfoX)) return false;
	//if (false == ReX.TravelOffset.updateFrom(InfoX)) return false;
	//// label
	//if (false == ReX.LabelMin.updateFrom(InfoX)) ReX.LabelMin = "";
	//if (false == ReX.LabelMax.updateFrom(InfoX)) ReX.LabelMax = "";
	//if (false == ReX.LabelMean.updateFrom(InfoX)) ReX.LabelMean = "";
	//if (false == ReX.LabelTravel.updateFrom(InfoX)) ReX.LabelTravel = "";
	//// comment
	//if (false == ReX.CommentLine1.updateFrom(InfoX)) ReX.CommentLine1 = "";
	//if (false == ReX.CommentLine2.updateFrom(InfoX)) ReX.CommentLine2 = "";

	//// consistency checks
	//if (ReX.SignalMax.asFloat() <= ReX.SignalMin.asFloat()) return false;

	//// initialize FTLight stream
	//CmFileFTLStream FTLightStream(ReX.FTLightUURI.getText(), ReX.FilePeriod.asInt32(), ReX.FileName.getText(), ReX.FTLightRoot.getText());

	//// decode strings which may change dynamically and which may still be encoded due to being the first item in a line
	//ReX.TimeStart.decodeTextX();
	//ReX.TimeEnd.decodeTextX();
	//ReX.ChartTitle.decodeTextX();
	//ReX.CommentLine1.decodeTextX();
	//ReX.CommentLine2.decodeTextX();

	//// set start/end time
	//CmTimestamp StartTime(CmDateTime::getNanoSec(ReX.TimeStart.getText()));
	//CmTimestamp EndTime(CmDateTime::getNanoSec(ReX.TimeEnd.getText()));
	//FTLightStream.setStartTime(StartTime,false);
	//FTLightStream.setEndTime(EndTime,false);

	//// initialize chart on viewer
	//CmRange AxisRangeX(0, (float)FTLightStream.getStreamLength((uint64)ReX.TimeDiRemote.asFloat()));
	//CmRange AxisRangeY(ReX.SignalMin.asFloat(), ReX.SignalMax.asFloat());
	//CmPoint2D UpperLeft;
	//CmPoint2D BottomRight;
	//Viewer->generateChartAxes(ReX.ChartTitle, AxisRangeX, AxisRangeY, UpperLeft, BottomRight, &ReX.TimeLabel, &ReX.SignalLabel, &ReX.FTLightUURI, &ReX.TimeStart, &ReX.TimeEnd, &ReX.CommentLine1, &ReX.CommentLine2);

	//// generate a data array for charting
	//int32 NumPointsX = (int32)(BottomRight.x() - UpperLeft.x() + 1);
	//int32 ValueRangeY = (int32)(BottomRight.y() - UpperLeft.y() - 2);

	//// pre-process all data lines into range values for chart columns
	//uint32 Lines = 0;
	//CmTimestamp Timestamp = StartTime;
	//double ChartStart = (double)StartTime.getTimestamp_ns();
	//double ChartEnd = (double)EndTime.getTimestamp_ns();
	//double ChartIncrement = (ChartEnd - ChartStart) / (NumPointsX - 1);

	///** cumulation of particular measurement values for one chart column
	// * Samples - number of samples in each data block
	// * First - first value
	// * Min - minimal value
	// * Max - maximal value
	// * Total - sum of all values
	// * Travel - sum of all value increments
	// */
	//uint32 ValueSamples = ReX.ValueSamples.asUInt32();
	//uint32 ValueColumns = ReX.ValueColumns.asUInt32();
	//uint32 ValueIndexFirst = ReX.ValueIndexFirst.asUInt32();
	//uint32 ValueIndexMin = ReX.ValueIndexMin.asUInt32();
	//uint32 ValueIndexMax = ReX.ValueIndexMax.asUInt32();
	//uint32 ValueIndexTotal = ReX.ValueIndexTotal.asUInt32();
	//uint32 ValueIndexTravel = ReX.ValueIndexTravel.asUInt32();
	//// verify
	//if (ValueSamples < 1) return false;
	//if (ValueColumns < 1) return false;
	//if (ValueIndexFirst >= ValueColumns) return false;
	//if (ValueIndexMin >= ValueColumns) return false;
	//if (ValueIndexMax >= ValueColumns) return false;
	//if (ValueIndexTotal >= ValueColumns) return false;
	//if (ValueIndexTravel >= ValueColumns) return false;
	//int32 *Count = new int32[NumPointsX];
	//double *First = new double[NumPointsX];
	//double *Min = new double[NumPointsX];
	//double *Max = new double[NumPointsX];
	//double *Total = new double[NumPointsX];
	//double *Travel = new double[NumPointsX];
	//// values from file
	//uint64 *Values = new uint64[ValueColumns];
	//CmString* pField;

	//// clear workspace
	//for (int32 x = 0; x < NumPointsX; x++){
	//	Count[x] = 0;
	//	First[x] = 0;
	//	Min[x] = 0;
	//	Max[x] = 0;
	//	Total[x] = 0;
	//	Travel[x] = 0;
	//}

	//// determine particular measurement values for all chart columns
	//for (int32 x = -1; x < NumPointsX; x++){
	//	// get values from a single sample sequence 
	//	while (FTLightStream.getNextTimestamp(Timestamp)){
	//		// get all values from one line
	//		Lines++;
	//		for (uint32 i = 0; i < ValueColumns; i++){
	//			if (NULL != (pField = FTLightStream.getNextField()))
	//				Values[i] = FTLightStream.BinX2num(*pField);
	//			else 
	//				Values[i] = 0;
	//		}
	//		// check if the timestamp belongs to current chart column
	//		if (Timestamp.getTimestamp_ns() >= ChartStart + x*ChartIncrement){
	//			// step to and initialize next chart column
	//			Count[x+1] = 1;
	//			First[x+1] = (double)Values[ValueIndexFirst];
	//			Min[x+1] = (double)Values[ValueIndexMin];
	//			Max[x+1] = (double)Values[ValueIndexMax];
	//			Total[x+1] = (double)Values[ValueIndexTotal];
	//			Travel[x+1] = (double)Values[ValueIndexTravel];
	//			break;
	//		}
	//		else{
	//			// integrate a measurement sequence in chart column data 
	//			Count[x] += 1;
	//			if (Min[x] > Values[ValueIndexMin]){
	//				Min[x] = (double)Values[ValueIndexMin];
	//			}
	//			if (Max[x] < Values[ValueIndexMax]){
	//				Max[x] = (double)Values[ValueIndexMax];
	//			}
	//			Total[x] += Values[ValueIndexTotal];
	//			Travel[x] += Values[ValueIndexTravel];
	//		}
	//	}
	//}

	//// generate chart points array
	//CmPoint2D *PointsMin = new CmPoint2D[NumPointsX];
	//CmPoint2D *PointsMax = new CmPoint2D[NumPointsX];
	//CmPoint2D *PointsMean = new CmPoint2D[NumPointsX];
	//CmPoint2D *PointsTravel = new CmPoint2D[NumPointsX];
	//double Offset = ReX.SignalOffset.asFloat() + ReX.SignalMin.asFloat();
	//double SignalRange = ReX.SignalMax.asFloat() - ReX.SignalMin.asFloat();
	//double ChartScale = fabs(UpperLeft.y() - BottomRight.y())/SignalRange;
	//double ChartBaseline = BottomRight.y() + ChartScale * Offset;
	//double ScaleMinMax = ChartScale * ReX.SignalScale.asFloat();
	//double ScaleMean = ScaleMinMax / ValueSamples;
	//double ScaleTravel = ReX.SignalScale.asFloat() / ValueSamples;
	//double TravelBaseline = BottomRight.y() + ChartScale * (ReX.SignalMin.asFloat() - ReX.TravelOffset.asFloat());
	//double TravelScaleFactor = ReX.TravelFactor.asFloat() * ChartScale;
	//for (int32 x = 0; x < NumPointsX; x++){
	//	PointsMax[x].x() = UpperLeft.x() + x;
	//	PointsMin[x].x() = UpperLeft.x() + x;
	//	PointsMean[x].x() = UpperLeft.x() + x;
	//	PointsTravel[x].x() = UpperLeft.x() + x;
	//	// change min/max dependent on positive/negative scale
	//	if (ScaleMinMax >= 0){
	//		PointsMax[x].y() = (float)(ChartBaseline - ScaleMinMax * Max[x]);
	//		PointsMin[x].y() = (float)(ChartBaseline - ScaleMinMax * Min[x]);
	//	}
	//	else{
	//		PointsMax[x].y() = (float)(ChartBaseline - ScaleMinMax * Min[x]);
	//		PointsMin[x].y() = (float)(ChartBaseline - ScaleMinMax * Max[x]);
	//	}
	//	// mean, travel
	//	if (Count[x] > 0){
	//		PointsMean[x].y() = (float)(ChartBaseline - ScaleMean * Total[x] / Count[x]);
	//		PointsTravel[x].y() = (float)(TravelBaseline - TravelScaleFactor * (ScaleTravel * Travel[x] / Count[x] - ReX.TravelBalance.asFloat()));
	//	}
	//	else{
	//		PointsMean[x].y() = 0;
	//		PointsTravel[x].y() = 0;
	//	}
	//}

	//// clear chart
	//Viewer->clearChart();
	//// polygons
	//float LineWidth = 1.0;
	//float LineAlpha = 1.0;
	//float FillAlpha = 0.0;
	//CmString FillColor("White");
	//CmString LineColor("Red");
	//Viewer->displayPolygonExt(PointsMax, NumPointsX, LineWidth, LineColor, LineAlpha, FillColor, FillAlpha);
	//LineColor = "Blue";
	//Viewer->displayPolygonExt(PointsMin, NumPointsX, LineWidth, LineColor, LineAlpha, FillColor, FillAlpha);
	//LineColor = "Black";
	//Viewer->displayPolygonExt(PointsMean, NumPointsX, LineWidth, LineColor, LineAlpha, FillColor, FillAlpha);
	//LineColor = "Magenta";
	//Viewer->displayPolygonExt(PointsTravel, NumPointsX, LineWidth, LineColor, LineAlpha, FillColor, FillAlpha);

	//// label settings
	//const float RelativeFontSize = 0.025f;
	//const float HorizontalOffset = 0.01f;
	//float FontSize = (float)max(7,fabs(UpperLeft.y() - BottomRight.y())*RelativeFontSize);
	//int32 Index = (int32)(NumPointsX * HorizontalOffset);
	//// max
	//CmString TextColor("Red");
	//float VerticalOffset = -2.0;
	//writeLabel(ReX.LabelMax, FontSize, TextColor, PointsMax, Index, VerticalOffset);
	//// min
	//TextColor = "Blue";
	//VerticalOffset = -2.0;
	//writeLabel(ReX.LabelMin, FontSize, TextColor, PointsMin, Index, VerticalOffset);
	//// mean
	//TextColor = "Black";
	//VerticalOffset = -2.0;
	//writeLabel(ReX.LabelMean, FontSize, TextColor, PointsMean, Index, VerticalOffset);
	//// travel
	//TextColor = "Magenta";
	//VerticalOffset = -4.0;
	//writeLabel(ReX.LabelTravel, FontSize, TextColor, PointsTravel, Index, VerticalOffset);


	//// cleanup
	//delete[] Count;
	//delete[] First;
	//delete[] Min;
	//delete[] Max;
	//delete[] Total;
	//delete[] Travel;
	//delete[] PointsMax;
	//delete[] PointsMin;
	//delete[] PointsMean;
	//delete[] PointsTravel;

	return true;
}

bool PROVIDER_CmRepositoryFTL::writeLabel(CmString /*_Text*/, float /*_FontSize*/, CmString /*_TextColor*/, CmPoint2D * /*_Points*/, int32 /*_Index*/, float /*_VerticalOffset*/)
{
	//// horizontal alignment
	//float PosX = _Points[_Index].x();

	//// vertical alignment
	//float PosY = _Points[_Index].y() + _VerticalOffset * _FontSize;

	//float Alpha = 1.0;
	//CmString Font("Arial");
	//Viewer->displayTextExt(_Text, CmPoint2D(PosX, PosY), 0, _FontSize, _TextColor, Alpha, Font);

	return true;
}
