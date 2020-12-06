//////////////////////////////////////////////////////////////////////////////
//
// AppCm.h: Declaration of AppCm application (App)
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

#ifndef AppCmH
#define AppCmH

#include "Services/Drawing/CmDrawing.h"
#include "Services/PlugNode/CmPlugNode.h"
#include "Bots/CmBots.h"


namespace AppCm
{

	#define COMPANY_AppCm					"WegaLink"
	#define CREATOR_AppCm					"EKD@JN58nc_Türkenfeld"
	#define PROGRAM_AppCm					"AppCm"
	#define VERSION_AppCm					"v1.0"
	#define CREATED_AppCm					"2020-03-30"
	// UURI 
	#define UURI_AppCm						CREATOR_AppCm"."PROGRAM_AppCm
	#define UURI_SERVICE_AppCm		"/"VERSION_AppCm
	#define UURI_PROVIDER_AppCm		UURI_SERVICE_AppCm"_"CREATED_AppCm

	// forward declarations
	class PROVIDER_AppCm;
	// application
	typedef struct _AppCmProfile AppCmProfile;
	typedef struct _AppCmDynamic AppCmDynamic;
	// communication
	typedef struct _AppCmControl AppCmControl;
	typedef struct _AppCmMessage AppCmMessage;
	typedef struct _AppCmPolling AppCmPolling;
	typedef struct _AppCmReading AppCmReading;

	// update GUI callback function
	typedef	bool(*UpdateGUI)(AppCmProfile& _pro, AppCmDynamic& _dyn);

//============================================================================
// SERVICE_AppCm
//============================================================================
//
class SERVICE_AppCm : public CmPlugNode
{

	// log level
#ifdef _DEBUG
	//#define CMLOG_AppCm CMLOG_None
	//#define CMLOG_AppCm CMLOG_Error
	#define CMLOG_AppCm CMLOG_QuickView
	//#define CMLOG_AppCm CMLOG_Info
	//#define CMLOG_AppCm CMLOG_Control
	//#define CMLOG_AppCm CMLOG_Process
	//#define CMLOG_AppCm CMLOG_Network
#else
	#define CMLOG_AppCm CMLOG_Error
#endif

public:
	// Constructor, Destructor
	SERVICE_AppCm();
	~SERVICE_AppCm();

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
	bool updateGUI(AppCmProfile *_pro = NULL, AppCmDynamic *_dyn = NULL);

	//------CONFIGURATION-MANAGEMENT-functions----------------------------------

public:
	/** Profile.
	*  The profile values will be returned/updated/written/read to/from disk
	*/
	AppCmProfile& getProfile();
	bool updateProfile(CmStringFTL& _ProFTL);
	bool writeProfile(CmString _ConfigPath);
	bool readProfile(CmString _ConfigPath);
	bool setDefaultProfile();

public:
	/** Dynamic.
	*   The dynamic values will be returned/updated/written.
	*/
	AppCmDynamic& getDynamic();
	bool updateDynamic(CmStringFTL& _dyn);
	bool writeDynamic(CmString _ConfigPath);

public:
	/** Control.
	*   The control values will be returned/updated/written.
	*/
	AppCmControl& getControl();
	bool updateControl(CmStringFTL& _ctr);
	bool writeControl(CmString _ConfigPath);

public:
	/** Message.
	*   The message (event) values will be returned/updated/written.
	*/
	AppCmMessage& getMessage();
	bool updateMessage(CmStringFTL& _msg);
	bool writeMessage(CmString _ConfigPath);

public:
	/** Polling.
	*   The polling values will be returned/updated/written.
	*/
	AppCmPolling& getPolling();
	bool updatePolling(CmStringFTL& _pln);
	bool writePolling(CmString _ConfigPath);

public:
	/** Reading.
	*   The reading values will be returned/updated/written.
	*/
	AppCmReading& getReading();
	bool updateReading(CmStringFTL& _rdn);
	bool writeReading(CmString _ConfigPath);

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
	PROVIDER_AppCm& Provider();

private:
	// A local instance of a PROVIDER that implements the SERVICE interface
	PROVIDER_AppCm *LocalProvider;
};


//============================================================================
// PROVIDER_AppCm.
//============================================================================
//
class PROVIDER_AppCm : public CmPlugNode 
{
public:
	PROVIDER_AppCm();
	~PROVIDER_AppCm();

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
	bool updateGUI(AppCmProfile *_pro = NULL, AppCmDynamic *_dyn = NULL);

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
	AppCmProfile& getProfile();
	bool updateProfile(CmStringFTL& _ProFTL);
	bool writeProfile(CmString _ConfigPath);
	bool readProfile(CmString _ConfigPath);
	bool setDefaultProfile();

public:
	// Dynamic
	AppCmDynamic& getDynamic();
	bool updateDynamic(CmStringFTL& _dyn);
	bool writeDynamic(CmString _ConfigPath);

public:
	// Control.
	AppCmControl& getControl();
	bool updateControl(CmStringFTL& _ctr);
	bool writeControl(CmString _ConfigPath);

public:
	// Message.
	AppCmMessage& getMessage();
	bool updateMessage(CmStringFTL& _msg);
	bool writeMessage(CmString _ConfigPath);

public:
	// Polling.
	AppCmPolling& getPolling();
	bool updatePolling(CmStringFTL& _pln);
	bool writePolling(CmString _ConfigPath);

public:
	// Reading.
	AppCmReading& getReading();
	bool updateReading(CmStringFTL& _rdn);
	bool writeReading(CmString _ConfigPath);

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
// AppCm_Config
//============================================================================
#pragma warning (disable : 4510 4610)

//------AppCm-profile-------------------------------------------------------
// persistent configuration values
struct _AppCmProfile {
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
	// bots 
	CmValueINI Bots;
	BOT_CmBots Bot1;

};

#ifdef AppCm_Config
// initialize profile
AppCmProfile pro =
{
	// identification
	UURI_AppCm"/Profile/"VERSION_AppCm,
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
	// bots 
	"/Bots:module functionality access",
	">Bot1:bot-1",

};
#endif

//------AppCm-dynamic-------------------------------------------------------
// volatile dynamic values
struct _AppCmDynamic {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	CmValueINI LogLevel;
	CmValueINI Message;
	CmValueINI Context;

};

#ifdef AppCm_Config
// initialize dynamics
AppCmDynamic dyn =
{
	// identification
	UURI_AppCm"/Dynamic/"VERSION_AppCm,
	"/timestamp",
	">loglevel",
	"message",
	"context",

};
#endif

//------AppCm-control-------------------------------------------------------
// control values
// module control information
struct _AppCmControl {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef AppCm_Config
// initialize control
AppCmControl ctr =
{
	// identification
	UURI_AppCm"/Control/"VERSION_AppCm,
	"/timestamp",

};
#endif

//------AppCm-message-------------------------------------------------------
// message values
// event information, sent values to be processed
#pragma warning (disable : 4510 4610)
struct _AppCmMessage {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef AppCm_Config
// initialize message
AppCmMessage msg =
{
	// identification
	UURI_AppCm"/Message/"VERSION_AppCm,
	"/timestamp",

};
#endif

//------AppCm-polling-------------------------------------------------------
// polling values
// initiate return of measurement values
#pragma warning (disable : 4510 4610)
struct _AppCmPolling {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef AppCm_Config
// initialize polling
AppCmPolling pln =
{
	// identification
	UURI_AppCm"/Polling/"VERSION_AppCm,
	"/timestamp",

};
#endif

//------AppCm-reading-------------------------------------------------------
// reading values
// response to polling, return measurement values
#pragma warning (disable : 4510 4610)
struct _AppCmReading {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef AppCm_Config
// initialize reading
AppCmReading rdn =
{
	// identification
	UURI_AppCm"/Reading/"VERSION_AppCm,
	"/timestamp",

};
#endif


} // namespace AppCm

using namespace AppCm;

#endif // !defined(AppCmH)
