//////////////////////////////////////////////////////////////////////////////
//
// CmRepositoryFTL.h: Declaration of RepositoryX classes
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

#ifndef CmRepositoryFTLH
#define CmRepositoryFTLH

#include "Services/PlugNode/CmPlugNode.h"
#include "FTLight/CmFileFTL.h"
//#include "Canvas/HexagonViewer/CmHexagonViewer.h"

namespace Cosmos
{

	/** CmRepositoryFTL
	*   Initialization string for displaying a test chart.
	*/
#define CMREPOSITORYX_TEST_CONTROL \
"EKD@JN58nc_Türkenfeld.RepositoryX\n\
,Command:test\n\
"

	/** CmRepositoryFTL
	*   Initialization string for displaying a chart from a RepositoryX.
	*/
#define CMREPOSITORYX_DEFAULT_CONTROL \
"EKD@JN58nc_Türkenfeld.RepositoryX,0\n\
,Command:display\n\
,Title:Solar radio flux\\, Türkenfeld/Germany\n\
,FTLight,UURI:EKD\\@JN58nc_Türkenfeld.Sat_11GHZ\n\
,,root:N\\:\\Eckhard\\FTLight\n\
,,File,name:daily.csv\n\
,,,period:86400\n\
,Value,samples:2000\n\
,,columns:10\n\
,Time,start:2015-06-09 12\\:00\\:00\n\
,,end:2015-06-14 12\\:00\\:00\n\
,,label:time [min]\n\
,,diRemote:60.0\n\
,Signal,label:[dB]\n\
,,scale:-0.00138716\n\
,,offset:-74.9\n\
,,max:8\n\
,,min:-8\n\
,Travel,balance:-0.1975\n\
,,factor:500\n\
,,offset:-5\n\
,Label,Min:min value\n\
,,Max:max value\n\
,,Mean:average\n\
,,Travel:travel (x500\\, spectral strength)\n\
,Comment,Line1:stationary dish 90 cm\\, standard LNC\\@11GHz\n\
,,Line2:aperture ~3°\\, azimut\\: 19°\\, altitude\\: 57°\n\
"

// information items' variables and addresses
#pragma warning (disable : 4510 4610)
typedef	struct {
	CmValueFTL UURI;
	CmValueFTL Timestamp;
	CmValueFTL Command;
	// chart
	CmValueFTL ChartTitle;
	// stream
	CmValueFTL FTLightUURI;
	CmValueFTL FTLightRoot;
	CmValueFTL FileName;
	CmValueFTL FilePeriod;
	// values
	CmValueFTL ValueSamples;
	CmValueFTL ValueColumns;
	CmValueFTL ValueIndexFirst;
	CmValueFTL ValueIndexMin;
	CmValueFTL ValueIndexMax;
	CmValueFTL ValueIndexTotal;
	CmValueFTL ValueIndexTravel;
	// time
	CmValueFTL TimeStart;
	CmValueFTL TimeEnd;
	CmValueFTL TimeLabel;
	CmValueFTL TimeDiRemote;
	// signal
	CmValueFTL SignalLabel;
	CmValueFTL SignalScale;
	CmValueFTL SignalOffset;
	CmValueFTL SignalMax;
	CmValueFTL SignalMin;
	// travel
	CmValueFTL TravelBalance;
	CmValueFTL TravelFactor;
	CmValueFTL TravelOffset;
	// label
	CmValueFTL LabelMin;
	CmValueFTL LabelMax;
	CmValueFTL LabelMean;
	CmValueFTL LabelTravel;
	// comment
	CmValueFTL CommentLine1;
	CmValueFTL CommentLine2;
	} _CmRepositoryFTL_InfoX;

#ifdef CmRepositoryFTL_InfoX
	_CmRepositoryFTL_InfoX ReX = {
	"0",					// UURI
	"0-0",				// Timestamp
	"0-1-0",			// Command
	// chart
	"0-2-0",			// ChartTitle
	// stream
	"0-3-0-0",		// FTLightUURI
	"0-3-1-0",		// FTLightRoot
	"0-3-2-0-0",	// FileName
	"0-3-2-1-0",	// FilePeriod
	// values
	"0-4-0-0",		// ValueSamples
	"0-4-1-0",		// ValueColumns
	"0-4-2-0",		// ValueIndexFirst
	"0-4-3-0",		// ValueIndexMin
	"0-4-4-0",		// ValueIndexMax
	"0-4-5-0",		// ValueIndexTotal
	"0-4-6-0",		// ValueIndexTravel
	// time
	"0-5-0-0",		// TimeStart
	"0-5-1-0",		// TimeEnd
	"0-5-2-0",		// TimeLabel
	"0-5-3-0",		// TimeDiRemote
	// signal
	"0-6-0-0",		// SignalLabel
	"0-6-1-0",		// SignalScale
	"0-6-2-0",		// SignalOffset
	"0-6-3-0",		// SignalMax
	"0-6-4-0",		// SignalMin
	// travel
	"0-7-0-0",		// TravelBalance
	"0-7-1-0",		// TravelFactor
	"0-7-2-0",		// TravelOffset
	// label
	"0-8-0-0",		// LabelMin
	"0-8-1-0",		// LabelMax
	"0-8-2-0",		// LabelMean
	"0-8-3-0",		// LabelTravel
	// comment
	"0-9-0-0",		// CommentLine1
	"0-9-1-0",		// CommentLine2
}
#endif
;


// forward declaration
class PROVIDER_CmRepositoryFTL;

//=============================================================================
// SERVICE_CmRepositoryFTL.
//=============================================================================
//
class SERVICE_CmRepositoryFTL : public CmPlugNode
{
// UURI extensions for a 'CmRepositoryFTL' SERVICE and PROVIDER
#define UURI_SERVICE_CmRepositoryFTL      "CmRepositoryFTL/version/1.0"
#define UURI_PROVIDER_CmRepositoryFTL     "/built/2015-03-21"

public:
  SERVICE_CmRepositoryFTL();
  ~SERVICE_CmRepositoryFTL();

//------PlugNode-connectivity--------------------------------------------------

public:
	/** getUURI.
	*  The UURI including version information will be retrieved from class
	*  implementation.
	*/
	virtual CmUURI& getUURI();

public:
	/** processInformation.
	*  An information item will be sent to and be processed by provider.
	*/
	bool processInformation(CmString& Information);

public:
	/** set/getViewer.
	*   A PowerView interface will be connected to a module.
	*   The get function returns currently connected viewer
	*/
	//bool setViewer(SERVICE_CmHexagonViewer *Viewer, int32 ViewerSlot);
	//SERVICE_CmHexagonViewer * getViewer();
	//int32 getViewerSlot();

//------RepositoryX-operation--------------------------------------------------


public:
	/** testChart.
	*   A test chart (moving sine curve) will be drawn on viewer.
	*/
	bool testChart();

	/** displayChart.
  *   A chart of a currently selected FTLight repository will be displayed 
  *   on the viewer for a determined time periode
  */
  bool displayChart();





//------Service-access-by-PROVIDER---------------------------------------------

private:
  /** Provider.
   *  A SERVICE PROVIDER will be returned which may be either a referenc to a
   *  local PROVIDER implementation or to a remote PROVIDER implementation. If
   *  there has not been a PROVIDER yet, then a local PROVIDER will be
   *  instantiated. It is supposed that this will usually succeed. Otherwise an
   *  exception will be thrown since it is not possible to continue program flow
   *  when instantiation of a class fails due to e.g. insufficient resources.
   */
  PROVIDER_CmRepositoryFTL& Provider();

  // A local instance of a PROVIDER that implements the SERVICE interface
  PROVIDER_CmRepositoryFTL *LocalProvider;

};



//=============================================================================
// PROVIDER_CmRepositoryFTL.
//=============================================================================
//
	class PROVIDER_CmRepositoryFTL : public SERVICE_CmRepositoryFTL
{
public:
  PROVIDER_CmRepositoryFTL();
  ~PROVIDER_CmRepositoryFTL();

//------PlugNode-connectivity-------------------------------------PROVIDER-----

public:
  // PROVIDER's implementation of getUURI.
  CmUURI& getUURI();

public:
	// process information
	bool processInformation(CmString& Information);

public:
	// set/get viewer
	//bool setViewer(SERVICE_CmHexagonViewer *Viewer, int32 ViewerSlot);
	//SERVICE_CmHexagonViewer * getViewer();
	//int32 getViewerSlot();

//------RepositoryX-operation-------------------------------------PROVIDER-----

public:
	// chart output
	bool testChart();
	bool displayChart();

private:
	bool writeLabel(CmString Text, float FontSize, CmString Color, CmPoint2D *Points, int32 Index, float VerticalOffset);

//------PlugNode-workspace----------------------------------------PROVIDER-----

private:
	// FTLight based communication
	CmStringFTL InfoX;

private:
	// viewer interface
	//int32 ViewerSlot;
	//SERVICE_CmHexagonViewer *Viewer;
	CmPoint2D UpperLeft;
	CmPoint2D BottomRight;
	CmPoint2D *Points_1;
	CmPoint2D *Points_2;


};

} // namespace Cosmos

#endif // !defined(CmRepositoryFTL)
