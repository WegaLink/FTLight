//////////////////////////////////////////////////////////////////////////////
//
// InfoNet.h: Declaration of InfoNet classes
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

#ifndef InfoNetH
#define InfoNetH

#include "Services/Drawing/CmDrawing.h"
#include "Services/PlugNode/CmPlugNode.h"

#define APP_COMPANY					"WegaLink"
#define APP_PROGRAM					"InfoNet"
#define APP_VERSION					"v1.0"

namespace InfoNet
{
	// forward declarations
	class PROVIDER_InfoNet;
	// application
	typedef struct _InfoNetProfile InfoNetProfile;
	typedef struct _InfoNetDynamic InfoNetDynamic;
	// communication
	typedef struct _InfoNetControl InfoNetControl;
	typedef struct _InfoNetPolling InfoNetPolling;
	typedef struct _InfoNetMeasure InfoNetMeasure;
	typedef struct _InfoNetDisplay InfoNetDisplay;

	// update GUI callback function
	typedef	bool(*UpdateGUI)(InfoNetProfile& _pro, InfoNetDynamic& _dyn);

//============================================================================
// SERVICE_InfoNet
//============================================================================
//
class SERVICE_InfoNet : public CmPlugNode
{
	// UURI extensions for a 'InfoNet' SERVICE and PROVIDER
#define UURI_InfoNet							"EKD@JN58nc_Türkenfeld."APP_PROGRAM
#define UURI_SERVICE_InfoNet			"/"APP_VERSION
#define UURI_PROVIDER_InfoNet			UURI_SERVICE_InfoNet"/built/2020-03-30"

	// log level
#ifdef _DEBUG
	//#define CMLOG_InfoNet CMLOG_None
	//#define CMLOG_InfoNet CMLOG_Error
	#define CMLOG_InfoNet CMLOG_QuickView
	//#define CMLOG_InfoNet CMLOG_Info
	//#define CMLOG_InfoNet CMLOG_Control
	//#define CMLOG_InfoNet CMLOG_Process
	//#define CMLOG_InfoNet CMLOG_Network
#else
#define CMLOG_InfoNet CMLOG_Error
	//#define CMLOG_InfoNet CMLOG_QuickView
	//#define CMLOG_InfoNet CMLOG_Info
#endif

public:
	// Constructor, Destructor
	SERVICE_InfoNet();
	~SERVICE_InfoNet();

	//------GUI-content---------------------------------------------------------

public:
	/** GUI messages.	*/
	bool processMessageGUI(CmString Key, CmString Text, double Value = 0);

	//------Remote-service-access-----------------------------------------------

public:
	/** processInformation.
	*  An information item will be processed on arrival. This funtion is supposed
	*  to further deal with received information respectively to reject it.
	*/
	bool processInformation(CmString& Information);
	
	//------GUI-update----------------------------------------------------------

public:
	/** GUI.
	*  The window/canvas position and size can be saved/retrieved. Further, The
	*  application GUI will be updated.
	*/
	bool setWindow(CmPoint _Position, CmSize _Size);
	bool getWindow(CmPoint& _Position, CmSize& _Size, CmSize _MinSize, CmSize _ScreenSize);
	bool setCanvas(CmSize _SizeLocal, CmSize _SizeRemote, CmSize _SizeTouch);
	bool getCanvas(CmSize& _SizeLocal, CmSize& _SizeRemote, CmSize& _SizeTouch);
	bool updateGUI(InfoNetProfile *_pro = NULL, InfoNetDynamic *_dyn = NULL);

	//------CONFIGURATION-MANAGEMENT-functions----------------------------------

public:
	/** Profile.
	*  The profile values will be returned/updated/written/read to/from disk
	*/
	InfoNetProfile& getProfile();
	bool updateProfile(CmStringFTL& _ProFTL);
	bool writeProfile(CmString _ConfigPath);
	bool readProfile(CmString _ConfigPath);
	bool setDefaultProfile();

public:
	/** Dynamic.
	*   The dynamic values will be returned/updated/written.
	*/
	InfoNetDynamic& getDynamic();
	bool updateDynamic(CmStringFTL& _dyn);
	bool writeDynamic(CmString _ConfigPath);

public:
	/** Control.
	*   The control values will be returned/updated/written.
	*/
	InfoNetControl& getControl();
	bool updateControl(CmStringFTL& _ctr);
	bool writeControl(CmString _ConfigPath);

public:
	/** Polling.
	*   The polling values will be returned/updated/written.
	*/
	InfoNetPolling& getPolling();
	bool updatePolling(CmStringFTL& _pol);
	bool writePolling(CmString _ConfigPath);

public:
	/** Measure.
	*   The measure values will be returned/updated/written.
	*/
	InfoNetMeasure& getMeasure();
	bool updateMeasure(CmStringFTL& _msr);
	bool writeMeasure(CmString _ConfigPath);

public:
	/** Display.
	*   The display (event) values will be returned/updated/written.
	*/
	InfoNetDisplay& getDisplay();
	bool updateDisplay(CmStringFTL& _dsp);
	bool writeDisplay(CmString _ConfigPath);

public:
	/** Synchronization / Diagnostics.
	*   The log values will be cleared.
	*/
	bool enterSerialize(int32 _LockID = 0);
	bool leaveSerialize();
	bool clearLogLevel();

public:
	/** getUURI.
	*  The UURI including version information will be retrieved from class
	*  implementation.
	*/
	CmUURI& getUURI();

//------Service-access-through-a-PROVIDER-------------------------------------

public:
	/** Provider.
	*  A SERVICE PROVIDER will be returned which may be either a referenc to a
	*  local PROVIDER implementation or to a remote PROVIDER implementation. If
	*  there has not been a PROVIDER yet, then a local PROVIDER will be
	*  instantiated. It is supposed that this will usually succeed. Otherwise an
	*  exception will be thrown since it is not possible to continue program flow
	*  when instantiation of a class fails due to e.g. insufficient resources.
	*/
	PROVIDER_InfoNet& Provider();

private:
	// A local instance of a PROVIDER that implements the SERVICE interface
	PROVIDER_InfoNet *LocalProvider;
};


//============================================================================
// PROVIDER_InfoNet.
//============================================================================
//
class PROVIDER_InfoNet : public CmPlugNode 
{
public:
	PROVIDER_InfoNet();
	~PROVIDER_InfoNet();

	//------GUI-content---------------------------------------------PROVIDER----

public:
	bool processMessageGUI(CmString _Key, CmString _Text, double _Value);

	//------GUI-helper-functions------------------------------------PROVIDER----

public:
	bool touchData(CmString& _KeyCode);
	bool drawLocal();
	bool drawRemote();
	bool drawTouch();
	bool setParallel(CmString& _Command);

	//------Remote-service-access-----------------------------------PROVIDER----

public:
	bool processInformation(CmString& _Information);

	//------GUI-update----------------------------------------------PROVIDER----

public:
	bool setWindow(CmPoint _Position, CmSize _Size);
	bool getWindow(CmPoint& _Position, CmSize& _Size, CmSize _MinSize, CmSize _ScreenSize);
	bool setCanvas(CmSize _SizeLocal, CmSize _SizeTouch, CmSize _SizeRemote);
	bool getCanvas(CmSize& _SizeLocal, CmSize& _SizeTouch, CmSize& _SizeRemote);
	bool updateGUI(InfoNetProfile *_pro = NULL, InfoNetDynamic *_dyn = NULL);

public:
	/** A callback to update the GUI will be registered respectively removed. */
	bool registerUpdateGUICallback(UpdateGUI _FuncUpdateGUI);
	bool unregisterUpdateGUICallback(UpdateGUI _FuncUpdateGUI);

private:
	// update GUI callback function
	UpdateGUI FuncUpdateGUI;
	bool isUpdateGUI;
	bool isShutDown;

	//------Background-data-processing------------------------------PROVIDER----

public:
	bool runParallel();

	//------CONFIGURATION-MANAGEMENT-functions----------------------PROVIDER----

public:
	// Profile
	InfoNetProfile& getProfile();
	bool updateProfile(CmStringFTL& _ProFTL);
	bool writeProfile(CmString _ConfigPath);
	bool readProfile(CmString _ConfigPath);
	bool setDefaultProfile();

public:
	// Dynamic
	InfoNetDynamic& getDynamic();
	bool updateDynamic(CmStringFTL& _dyn);
	bool writeDynamic(CmString _ConfigPath);

public:
	// Control.
	InfoNetControl& getControl();
	bool updateControl(CmStringFTL& _ctr);
	bool writeControl(CmString _ConfigPath);

public:
	// Polling.
	InfoNetPolling& getPolling();
	bool updatePolling(CmStringFTL& _pol);
	bool writePolling(CmString _ConfigPath);

public:
	// Measure.
	InfoNetMeasure& getMeasure();
	bool updateMeasure(CmStringFTL& _msr);
	bool writeMeasure(CmString _ConfigPath);

public:
	// Display.
	InfoNetDisplay& getDisplay();
	bool updateDisplay(CmStringFTL& _dsp);
	bool writeDisplay(CmString _ConfigPath);

public:
	// Diagnostics.
	bool clearLogLevel();
	// Return a CmValueFTL& of a LogLevel, Message, Context sequence
	CmValueINI& getReturn();

public:
	// PROVIDER's implementation of getUURI.
	CmUURI& getUURI();

	//--------------workspace---------------------------------------------------

private:
	bool isDraw;

};

//============================================================================
// InfoNet_Config
//============================================================================
#pragma warning (disable : 4510 4610)

//------InfoNet-profile-------------------------------------------------------
// persistent configuration values
struct _InfoNetProfile {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	// window
	CmValueINI Window;
	CmValueINI WindowTop;
	CmValueINI WindowLeft;
	CmValueINI WindowWidth;
	CmValueINI WindowHeight;
	// local 
	CmValueINI Local;
	CmValueINI LocalDevice;
	CmChart		 LocalChart;
	CmValueINI LocalChartBackgroundColor;
	CmValueINI LocalChartFontSize;
	CmValueINI LocalChartFontColor;
	CmValueINI LocalChartSignalColor;
	// remote 
	CmValueINI Remote;
	CmValueINI RemoteDevice;
	CmChart		 RemoteChart;
	CmValueINI RemoteChartBackgroundColor;
	CmValueINI RemoteChartFontSize;
	CmValueINI RemoteChartFontColor;
	CmValueINI RemoteChartSignalColor;
	// touch 
	CmValueINI Touch;
	CmValueINI TouchMode;
	CmChart		 TouchChart;
	CmValueINI TouchChartBackgroundColor;
	CmValueINI TouchChartFontSize;
	CmValueINI TouchChartFontColor;
	CmValueINI TouchChartSignalColor;

};

#ifdef InfoNet_Config
// initialize profile
InfoNetProfile pro =
{
	// identification
	UURI_InfoNet"/Profile/"APP_VERSION,
	"/timestamp",
	// window
	"/Window:window position/size",
	">Top:100",
	"Left:100",
	"Width:1046",
	"Height:840",
	// local
	"/Local:local canvas settings",
	">Device:OFF",
	"Chart:Local Chart Title",
	">BackgroundColor:DarkSeaGreen",
	"FontSize:12",
	"FontColor:Black",
	"SignalColor:Red",
	// Remote
	"/Remote:remote canvas settings",
	">Device:OFF",
	"Chart:Remote Chart Title",
	">BackgroundColor:LightSteelBlue",
	"FontSize:12",
	"FontColor:Black",
	"SignalColor:CornflowerBlue",
	// touch
	"/Touch:touch mode settings",
	">Mode:TEST",
	"Chart:Touch Chart Title",
	">BackgroundColor:Wheat",
	"FontSize:12",
	"FontColor:Black",
	"SignalColor:Orange",

};
#endif

//------InfoNet-dynamic-------------------------------------------------------
// volatile dynamic values
struct _InfoNetDynamic {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	CmValueINI LogLevel;
	CmValueINI Message;
	CmValueINI Context;

};

#ifdef InfoNet_Config
// initialize dynamics
InfoNetDynamic dyn =
{
	// identification
	UURI_InfoNet"/Dynamic/"APP_VERSION,
	"/timestamp",
	">loglevel",
	"message",
	"context",

};
#endif

//------InfoNet-control-------------------------------------------------------
// control values
// module control information
struct _InfoNetControl {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef InfoNet_Config
// initialize control
InfoNetControl ctr =
{
	// identification
	UURI_InfoNet"/Control/"APP_VERSION,
	"/timestamp",

};
#endif

//------InfoNet-polling-------------------------------------------------------
// polling values
// initiate return of measurement values
#pragma warning (disable : 4510 4610)
struct _InfoNetPolling {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef InfoNet_Config
// initialize polling
InfoNetPolling pol =
{
	// identification
	UURI_InfoNet"/Polling/"APP_VERSION,
	"/timestamp",

};
#endif

//------InfoNet-measure-------------------------------------------------------
// measure values
// response to polling, return measurement values
#pragma warning (disable : 4510 4610)
struct _InfoNetMeasure {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef InfoNet_Config
// initialize measure
InfoNetMeasure msr =
{
	// identification
	UURI_InfoNet"/Measure/"APP_VERSION,
	"/timestamp",

};
#endif

//------InfoNet-display-------------------------------------------------------
// display values
// event information, sent values to be displayed
#pragma warning (disable : 4510 4610)
struct _InfoNetDisplay {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef InfoNet_Config
// initialize display
InfoNetDisplay dsp =
{
	// identification
	UURI_InfoNet"/Display/"APP_VERSION,
	"/timestamp",

};
#endif


} // namespace InfoNet

using namespace InfoNet;

#endif // !defined(InfoNetH)
