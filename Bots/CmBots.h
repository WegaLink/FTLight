//////////////////////////////////////////////////////////////////////////////
//
// CmBots.h: Declaration of CmBots classes
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

#ifndef CmBotsH
#define CmBotsH

#include "Services/PlugNode/CmPlugNode.h"
#include "FTLight/CmValueFTL.h"

namespace Cosmos
{

	#define COMPANY_CmBots			"WegaLink"
	#define CREATOR_CmBots			"EKD@JN58nc_Türkenfeld"
	#define PROGRAM_CmBots			"CmBots"
	#define VERSION_CmBots			"v1.0"
	#define CREATED_CmBots			"2020-12-06"
	// UURI
	#define UURI_CmBots							CREATOR_CmBots"."PROGRAM_CmBots
	#define UURI_SERVICE_CmBots			UURI_CmBots"/"VERSION_CmBots
	#define UURI_PROVIDER_CmBots		UURI_SERVICE_CmBots"_"CREATED_CmBots

	// forward declarations
	class PROVIDER_CmBots;
	// bot
	typedef struct _CmBotsProfile CmBotsProfile;
	typedef struct _CmBotsDynamic CmBotsDynamic;
	// communication
	typedef struct _CmBotsControl CmBotsControl;
	typedef struct _CmBotsMessage CmBotsMessage;
	typedef struct _CmBotsPolling CmBotsPolling;
	typedef struct _CmBotsReading CmBotsReading;

//============================================================================
// SERVICE_CmBots.
//============================================================================
//
class SERVICE_CmBots : public CmPlugNode
{

public:
	SERVICE_CmBots();
	~SERVICE_CmBots();

	//------Bot-test------------------------------------------------------------

public:
	/** testBot.
	*  This function runs a bot test against implemented functionality.
	*/
	bool testBot();

	//------Remote-service-access-----------------------------------------------

public:
	/** processInformation.
	*  An information item will be processed on arrival. This funtion is supposed
	*  to further deal with received information respectively to reject it. Any
	*  remote access/control functionality is provided by this function.
	*/
	bool processInformation(CmString& Information);

	//------Bot-functionality---------------------------------------------------






	//------CONFIGURATION-MANAGEMENT-functions----------------------------------

public:
	/** Profile.
	*  The profile values will be returned/updated/written/read to/from disk
	*/
	CmBotsProfile& pro();
	bool updateProfile(CmStringFTL& _ProFTL);
	bool writeProfile(CmString _ConfigPath);
	bool readProfile(CmString _ConfigPath);
	bool setDefaultProfile();

public:
	/** Dynamic.
	*   The dynamic values will be returned/updated/written.
	*/
	CmBotsDynamic& dyn();
	bool updateDynamic(CmStringFTL& _dyn);
	bool writeDynamic(CmString _ConfigPath);

public:
	/** Control.
	*   The control values will be returned/updated/written.
	*/
	CmBotsControl& ctr();
	bool updateControl(CmStringFTL& _ctr);
	bool writeControl(CmString _ConfigPath);

public:
	/** Message.
	*   The message (event) values will be returned/updated/written.
	*/
	CmBotsMessage& msg();
	bool updateMessage(CmStringFTL& _msg);
	bool writeMessage(CmString _ConfigPath);

public:
	/** Polling.
	*   The polling values will be returned/updated/written.
	*/
	CmBotsPolling& pln();
	bool updatePolling(CmStringFTL& _pln);
	bool writePolling(CmString _ConfigPath);

public:
	/** Reading.
	*   The reading values will be returned/updated/written.
	*/
	CmBotsReading& rdn();
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
	/** setBotName/getBotUURI
	*   A bot name will be added to the ServicUURI for getting a BotUURI.
	*/
	CmString& setBotName(CmString _BotName);
	CmString& getBotUURI();

public:
	/** getUURI.
	*  The UURI including version information will be retrieved from class
	*  implementation.
	*/
	virtual CmUURI& getUURI();

//------Service-access-by-PROVIDER--------------------------------------------

private:
  /** Provider.
   *  A SERVICE PROVIDER will be returned which may be either a referenc to a
   *  local PROVIDER implementation or to a remote PROVIDER implementation. If
   *  there has not been a PROVIDER yet, then a local PROVIDER will be
   *  instantiated. It is supposed that this will usually succeed. Otherwise an
   *  exception will be thrown since it is not possible to continue program run
   *  when instantiation of a class fails due to e.g. insufficient resources.
   */
	PROVIDER_CmBots& Provider();

  // A local instance of a PROVIDER that implements the SERVICE interface
	PROVIDER_CmBots *LocalProvider;

};

//============================================================================
// BOT_CmBots.
//============================================================================
//
class BOT_CmBots : public SERVICE_CmBots, public CmValueINI
{
public:
	BOT_CmBots(const int8 *_Init);
	~BOT_CmBots();
};

//============================================================================
// PROVIDER_CmBots.
//============================================================================
//
class PROVIDER_CmBots : public CmPlugNode
{
public:
	PROVIDER_CmBots();
	~PROVIDER_CmBots();

	//------Bot-test------------------------------------------------PROVIDER----

public:
	bool testBot();

	//------Remote-service-access-----------------------------------PROVIDER----

public:
	bool processInformation(CmString& _Information);

	//------Background-data-processing------------------------------PROVIDER----

public:
	bool runParallel();

	//------Bot-functionality---------------------------------------PROVIDER----

	// ToDo...


	//------CONFIGURATION-MANAGEMENT-functions----------------------PROVIDER----

public:
	// Profile
	CmBotsProfile& pro();
	bool updateProfile(CmStringFTL& _ProFTL);
	bool writeProfile(CmString _ConfigPath);
	bool readProfile(CmString _ConfigPath);
	bool setDefaultProfile();

public:
	// Dynamic
	CmBotsDynamic& dyn();
	bool updateDynamic(CmStringFTL& _dyn);
	bool writeDynamic(CmString _ConfigPath);

public:
	// Control.
	CmBotsControl& ctr();
	bool updateControl(CmStringFTL& _ctr);
	bool writeControl(CmString _ConfigPath);

public:
	// Message.
	CmBotsMessage& msg();
	bool updateMessage(CmStringFTL& _msg);
	bool writeMessage(CmString _ConfigPath);

public:
	// Polling.
	CmBotsPolling& pln();
	bool updatePolling(CmStringFTL& _pln);
	bool writePolling(CmString _ConfigPath);

public:
	// Reading.
	CmBotsReading& rdn();
	bool updateReading(CmStringFTL& _rdn);
	bool writeReading(CmString _ConfigPath);

public:
	// Diagnostics.
	bool clearLogLevel();
	// Return a CmValueINI& of a LogLevel, Message, Context sequence
	CmValueINI& getReturn();

public:
	// PROVIDER's implementation of getUURI.
	CmUURI& getUURI();

//------workspace-------------------------------------------------PROVIDER----

private:
	// config
	CmBotsProfile* Pro;
	CmBotsDynamic* Dyn;
	CmBotsControl* Ctr;
	CmBotsMessage* Msg;
	CmBotsPolling* Pln;
	CmBotsReading* Rdn;

};

//============================================================================
// CmBots_Config
//============================================================================
#pragma warning (disable : 4510 4610)

//------CmBots-profile--------------------------------------------------------
// persistent configuration values
struct _CmBotsProfile {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef CmBots_Config
// initialize profile
CmBotsProfile pro_ =
{
	// identification
	UURI_CmBots"/Profile/"VERSION_CmBots,
	"/timestamp",

};
#endif


//------CmBots-dynamic--------------------------------------------------------
// volatile dynamic values
struct _CmBotsDynamic {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;
	CmValueINI LogLevel;
	CmValueINI Message;
	CmValueINI Context;

};

#ifdef CmBots_Config
// initialize dynamics
CmBotsDynamic dyn_ =
{
	// identification
	UURI_CmBots"/Dynamic/"VERSION_CmBots,
	"/timestamp",
	">loglevel",
	">message",
	">context",

};
#endif

//------CmBots-control--------------------------------------------------------
// control values
// bot control information
struct _CmBotsControl {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef CmBots_Config
// initialize control
CmBotsControl ctr_ =
{
	// identification
	UURI_CmBots"/Control/"VERSION_CmBots,
	"/timestamp",

};
#endif

//------CmBots-message--------------------------------------------------------
// message values
// event information, sent values to be processed
struct _CmBotsMessage {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef CmBots_Config
// initialize message
CmBotsMessage msg_ =
{
	// identification
	UURI_CmBots"/Message/"VERSION_CmBots,
	"/timestamp",

};
#endif

//------CmBots-polling--------------------------------------------------------
// polling values
// initiate return of measurement values
struct _CmBotsPolling {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef CmBots_Config
// initialize polling
CmBotsPolling pln_ =
{
	// identification
	UURI_CmBots"/Polling/"VERSION_CmBots,
	"/timestamp",

};
#endif

//------CmBots-reading--------------------------------------------------------
// reading values
// response to polling, return measurement values
struct _CmBotsReading {
	// identification
	CmValueINI UURI;
	CmValueINI Timestamp;

};

#ifdef CmBots_Config
// initialize reading
CmBotsReading rdn_ =
{
	// identification
	UURI_CmBots"/Reading/"VERSION_CmBots,
	"/timestamp",

};
#endif


} // namespace Cosmos

using namespace Cosmos;

#endif // !defined(CmBotsH)
