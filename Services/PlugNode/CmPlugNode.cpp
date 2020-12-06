//////////////////////////////////////////////////////////////////////////////
//
// CmPlugNode.cpp: Implementation of the CmPlugNode classes
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

#include "Services/PlugNode/CmPlugNode.h"
#include "Services/PlugNode/CmGateway.h"
#include "Services/Basic/CmTime.h"

using namespace Cosmos;

// ContactPlugNode: a PlugNode known to others for logging in to a local network
CmPlugNode *ContactPlugNode = NULL;
int64 LocalContactID = 0;

// TEST
CmString Log;

//------------------------------------------------------------------------------
// CmPlugNode class
//------------------------------------------------------------------------------

CmPlugNode::CmPlugNode(CmUURI *_UURI,CmUURI *_RootUURI)
{
  // Initialize
  initPlugNode(_UURI,_RootUURI);

}
CmPlugNode::CmPlugNode(const char *_UURI,const char *_RootUURI)
{
  // Initialize
  CmUURI *temp_RootUURI = NULL==_RootUURI?NULL : new CmUURI("",_RootUURI);
  CmUURI *temp_UURI = NULL==_UURI?NULL : new CmUURI;
  if( NULL!= temp_UURI && NULL != _UURI )
    *temp_UURI = _UURI;
  initPlugNode(temp_UURI,temp_RootUURI);

  // Cleanup
  if( NULL!=temp_UURI )
    delete temp_UURI;
  if( NULL!=temp_RootUURI )
    delete temp_RootUURI;
}

CmPlugNode::CmPlugNode(const CmPlugNode& _PlugNode)
{
  initPlugNode(_PlugNode);
}

CmPlugNode::~CmPlugNode()
{
	LOG3("ID=", ContactID, " del CmPLugNode", Msg, CMLOG_Resource)

  // Disconnect this PlugNode from all local peers
	disconnectFromPeers();

  // Remove ContactPlugNode if this PlugNode represents the contact
  if( this==ContactPlugNode )
  {
    ContactPlugNode = NULL;
		LocalContactID = 0;

    // Close Windows sockets and free all resources
    WSACleanup();

		// delete all remaining connections
		deleteConnections();
  }

  // ToDo: Assign a new ContactPlugNode (if needed)
}

CmUURI& CmPlugNode::getUURI()
{
	return ServiceUURI;
}

CmString& CmPlugNode::setBotName(CmString& _BotName)
{
	// add module name to service UURI for getting a module UURI
	BotUURI = ServiceUURI.getText();
	BotUURI += "[";
	BotUURI += _BotName;
	BotUURI += "]";

	return BotUURI;
}
CmString& CmPlugNode::getBotUURI()
{
	return BotUURI;
}


// unit test
bool CmPlugNode::testPlugNode()
{
#define UURI_TEST_CmPlugNode "CmPlugNode_ID"
	const int PLUGNODE_COUNT_MAX = 5; 
	const bool isWriteLog = false;

	// test log
	Log = "\nTest CmPLugNode ";
	Log += CmDateTime::getTimeUTC();
	Log += "\nNOTE: PlugNodes ID=1,.. get sometimes disconnected (algorithm weakness?).";
	Log += "\n\nTest create & search";
	Log += "\n--------------------";

	// variables
	bool isSuccess = true;
	CmTimestamp Time;
	CmString Service;
	CmString Message;
	CmUURI UURI;

	LOG3("---- GENERATE an array (local network) of ", PLUGNODE_COUNT_MAX, " PlugNodes ----", Msg1, CMLOG_Info)

	CmPlugNode* PlugNodes[PLUGNODE_COUNT_MAX];
	Time.startRuntime();
	for (int n = 1; n <= PLUGNODE_COUNT_MAX; n++){
		// reset PlugNodes' contact ID
		LocalContactID = 0;
		// generate array of PlugNodes
		int Cycles = 0;
		Time.startRuntime();
		for (int i = 0; i < n; i++){
			Service = UURI_TEST_CmPlugNode;
			Service += i + 1;
			PlugNodes[i] = new CmPlugNode(Service.getText());
		}
		Message = n < 10 ? "\n  " : n < 100 ? "\n " : "\n";
		Message += n;
		Message += " CmPlugNodes created:              [us] ";
		Message += Time.getRuntime_us() / n;
		
		LOG1("---- FIND all PlugNodes ----", Msg2, CMLOG_Info)
		Message += "\n     find each PlugNode:";
		bool isSuccess = true;
		Time.startRuntime();
		for (int i = 0; i < n; i++){
			UURI = *PlugNodes[i]->getServiceUURI();
			if (NULL == PlugNodes[n-1]->findServiceUURI(UURI)){
				Message += "\n     - PlugNode not found ID= ";
				Message += PlugNodes[i]->getContactID();
				isSuccess = false;
			}
			Cycles++;
		}
		if (isSuccess){
			Message += "              [us]      ";
			Message += Time.getRuntime_us() / Cycles;
		}

		LOG1("---- DELETE PlugNodes ----", Msg3, CMLOG_Info)
		Message += "\n      delete PlugNodes:";
		Time.startRuntime();
		for (int i = 0; i < n; i++){
			delete PlugNodes[i];
		}
		
		LOG1("---- DELETE remaining connections ----", Msg4, CMLOG_Info)
		CmPlugNode::deleteConnections();
		Message += "               [us]         ";
		Message += Time.getRuntime_us() / n;
		if (false==isSuccess || n < 2 || n > PLUGNODE_COUNT_MAX - 1 || n%50==0){
			Log += Message;
			printf(Log.getText());
		}
		Log = "";
	}

	LOG1("---- TEST PlugNodes communication ----",Msg5,CMLOG_Info)
	const int32 ComCyclesLocal = 100;
	const int32 ComCyclesRemote = 5;
	Log += "\n\nTest communication";
	Log += "\n------------------";

	// create client and server PlugNodes
	CmString ClientInfo = getServiceUURI()->getText();
	CmString ServerInfo = getServiceUURI()->getText();
	ClientInfo += "/Client";
	ServerInfo += "/Server";
	SERVICE_CmPlugNode_Demo Client(NULL,ClientInfo.getBuffer());
	SERVICE_CmPlugNode_Demo Server(NULL,ServerInfo.getBuffer());
	CmUURI ServerUURI = *Server.getServiceUURI();
	CmUURI ClientUURI = *Client.getServiceUURI();
	LOG3("---- COMMUNICATE with local PlugNodes (", ComCyclesLocal, " loops) ----", Msg6, CMLOG_Info)
	CmString ClientInfoSend;
	CmString ServerInfoSend;
	CmString ClientInfoLast;
	CmString ServerInfoLast;
	CmString EmptyString;
	Log += "\n sendInfo() local:                    ";
	Time.startRuntime();
	for (int32 c = 0; c < ComCyclesLocal; c++){
		ClientInfoSend = ClientInfo;
		ClientInfoSend += c;
		ServerInfoSend = ServerInfo;
		ServerInfoSend += c;
		Client.sendInfo(ClientInfoSend, ServerUURI);
		Server.sendInfo(ServerInfoSend, ClientUURI);
		if (Server.getLastInfo() != ClientInfoSend){
			isSuccess = false;
			Log += "\n   server-";
			Log += c;
			Log += ":  '";
			Log += Server.getLastInfo();
			Log += "'\n   not equal: '";
			Log += ClientInfoSend;
			Log += "'";
		}
		if (Client.getLastInfo() != ServerInfoSend){
			isSuccess = false;
			Log += "\n   client-";
			Log += c;
			Log += ":  '";
			Log += Client.getLastInfo();
			Log += "'\n   not equal: '";
			Log += ClientInfoSend;
			Log += "'";
		}
	}
	Log += "[us]  ";
	Log += Time.getRuntime_us() / (2*ComCyclesLocal);

	LOG3("---- COMMUNICATE with remote PlugNodes (", ComCyclesRemote, " loops) ----", Msg7, CMLOG_Info)
	ServerUURI = *Server.getServiceUURI();
	ClientUURI = *Client.getServiceUURI();
	CmUURI NetworkUURI("testPlugNode",CM_NETWORK_UURI_LAN_TCP);
	Server.setNetworkUURI(NetworkUURI);
	Client.setNetworkUURI(NetworkUURI);
	Log += "\n sendInfo() remote:         ";
	// get server online by sending an info to itself
	ServerInfoSend = ServerInfo;
	ServerInfoSend += 0;
	if (false == Server.sendInfo(ServerInfoSend, ServerUURI, NetworkUURI)){
		isSuccess = false;
		Log += "\n   server startup failed.";
	}	else{
		Sleep(1); // thread change
		Time.startRuntime();
		for (int32 c = 1; c <= ComCyclesRemote; c++){
			ClientInfoSend = ClientInfo;
			ClientInfoSend += c;
			ServerInfoSend = ServerInfo;
			ServerInfoSend += c;
			// mutual information exchange between client and server
			Client.processInformation(EmptyString);
			Server.processInformation(EmptyString);
			bool isClientSendOK = Client.sendInfo(ServerInfoSend, ServerUURI, NetworkUURI);
			Sleep(5); // wait for exchange finished
			bool isServerSendOK = Server.sendInfo(ClientInfoSend, ClientUURI, NetworkUURI);
			Sleep(5); // wait for exchange finished
			if (Server.getLastInfo() != ServerInfoSend){
				isSuccess = false;
				isClientSendOK ? Log += "\n   server-" : Log += "\n   Client.sendInfo() failed.";
				isClientSendOK ? Log += c : 0;
				isClientSendOK ? Log += ":  '" : 0;
				isClientSendOK ? Log += Server.getLastInfo() : 0;
				isClientSendOK ? Log += "'\n   not equal: '" : 0;
				isClientSendOK ? Log += ServerInfoSend : 0;
				isClientSendOK ? Log += "'" : 0;
			}
			if (Client.getLastInfo() != ClientInfoSend){
				isSuccess = false;
				isServerSendOK ? Log += "\n   client-" : Log += "\n   Server.sendInfo() failed.";
				isServerSendOK ? Log += c : 0;
				isServerSendOK ? Log += ":  '" : 0;
				isServerSendOK ? Log += Client.getLastInfo() : 0;
				isServerSendOK ? Log += "'\n   not equal: '" : 0;
				isServerSendOK ? Log += ClientInfoSend : 0;
				isServerSendOK ? Log += "'" : 0;
			}
		}
		Log += "          [us]  ";
		Log += Time.getRuntime_us() / (2*ComCyclesRemote);
	}

	// put log message on screen
	printf(Log.getText());

	// write logfile to disk
	isWriteLog ? writeLog("CmPlugNode.log", "ekd@JN58nc_Türkenfeld.CmPlugNode") : 0;

	return isSuccess;
}

void CmPlugNode::initPlugNode(CmUURI *_UURI, CmUURI *_RootUURI)
{
	CmString RootUURI;
	CmString UURI;

  // Check for valid UURI
  if( NULL!=_UURI && NULL!=_RootUURI )
  {
    // This can be an arbitrary UURI with any RootUURI
		UURI = _UURI->getText();
		RootUURI = _RootUURI->getText();
		this->ServiceUURI.setUURI(_UURI->getText(), RootUURI.getText());
  }
  else
  if( NULL!=_UURI )
  {
    // Assume that UURI is a 'Cosmos' SERVICE. Therefore use the Cosmos RootUURI
		UURI = _UURI->getText();
		RootUURI = CM_UURI_ROOT_COSMOS;
		this->ServiceUURI.setUURI(_UURI->getText(), RootUURI.getText());
  }
  else
  if( NULL!=_RootUURI )
  {
    // Assume that RootUURI represents already a complete UURI
		UURI = "";
		RootUURI = _RootUURI->getText();
		this->ServiceUURI = *_RootUURI;
  }
  else
  {
    // Initialize CmPlugNode as a 'Cosmos' SERVICE using the 'Cosmos' RootUURI
		UURI = UURI_SERVICE_CmPlugNode;
		RootUURI = CM_UURI_ROOT_COSMOS;
		this->ServiceUURI.setUURI(UURI_SERVICE_CmPlugNode, CM_UURI_ROOT_COSMOS);
  }

  // Further PlugNode initialization
  initPlugNode(*this);
}

void CmPlugNode::initPlugNode(const CmPlugNode& _PlugNode)
{
	// Initialize workspace
	ServiceProvider = NULL;
	ContactID = 0;

	// Get service UURI
	CmUURI UURI = _PlugNode.ServiceUURI;
  ServiceUURI.setUURI(UURI);

	// Assign the ServiceUURI also to the LocalService base class
	CmServiceAccess::setServiceUURI(&this->ServiceUURI);
	// Assign the ServiceUURI also as BotUURI
	BotUURI = ServiceUURI.getText();

  // Take care for a first PlugNode
  if( NULL==ContactPlugNode )
  {
    ContactPlugNode = this;
		setContactID(ContactID);

    // Initialize socket subsystem
    WORD wVersionRequested = MAKEWORD(2,0);
    WSADATA wsaData;

    if( NOERROR == WSAStartup( wVersionRequested, &wsaData ) )
    {
      // Confirm that the WinSock DLL supports 2.0. or higher versions
      if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 )
      {
          // We couldn't find a usable WinSock DLL
          WSACleanup();
      }
    }
  }

  // Login to local network if the contact PlugNode is different from this one
  if( NULL!=ContactPlugNode && this!=ContactPlugNode )
  {
    CmUURI Cosmos;
    Cosmos = CM_UURI_ROOT_COSMOS;
		ContactPlugNode->checkinToLocalNetwork(Cosmos, this);
  }

	LOG4("ID=", ContactID, " new CmPLugNode UURI=", getServiceUURI()->getText(), Msg, CMLOG_PlugNodes)
}

bool CmPlugNode::checkinToLocalNetwork(CmUURI& _LocalNetworkUURI, CmPlugNode *_PlugNode)
{
	// NOTE: This method maintains a unique ContactID for each local PlugNode.
	//       Therefore it should be called only inside the unique ContactPlugNode. 

  // Connect to local ServiceAccess
	NetworkUURI = CM_UURI_ROOT_COSMOS;
	CmConnectionInfo ConnectionInfo;
  ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_ADR;
	ConnectionInfo.ADR.ContactAddress = _PlugNode;
	// NOTE: this should go into a critical section in order to avoid
	//       concurrency problems with multi-threading
	ConnectionInfo.ADR.ContactID = ++LocalContactID;

	return CmServiceAccess::joinNetwork(_LocalNetworkUURI, &ConnectionInfo);
}

bool CmPlugNode::reconnectNetwork()
{
	// Login to local network if the contact PlugNode is different from this one
	if (NULL != ContactPlugNode && this != ContactPlugNode)
	{
		CmUURI Cosmos;
		Cosmos = CM_UURI_ROOT_COSMOS;
		return ContactPlugNode->checkinToLocalNetwork(Cosmos, this);
	}
	return true;
}


//------------------------------------------------------------------------------
// SERVICE_CmPlugNode_Demo
//------------------------------------------------------------------------------
SERVICE_CmPlugNode_Demo::SERVICE_CmPlugNode_Demo(const char *_UURI, const char *_RootUURI)
	: CmPlugNode(_UURI, _RootUURI)
{
  // Initialize
  LocalProvider = NULL;

	LOG3("ID=", ContactID, " new SERVICE_CmPlugNode_Demo", Msg, CMLOG_PlugNodes)
}

SERVICE_CmPlugNode_Demo::~SERVICE_CmPlugNode_Demo()
{
	LOG3("ID=", ContactID, " del SERVICE_CmPlugNode_Demo", Msg, CMLOG_Resource)

  // Delete a local SERVICE implementation (PROVIDER) if it exists
	if (NULL != LocalProvider)
		delete LocalProvider;
}

CmUURI& SERVICE_CmPlugNode_Demo::getUURI()
{
	return Provider().getUURI();
}

bool SERVICE_CmPlugNode_Demo::processInformation(CmString& _Information)
{
	return Provider().processInformation(_Information);
}

const CmString& SERVICE_CmPlugNode_Demo::getLastInfo()
{
	return Provider().getLastInfo();
}

//------Service-access-through-a-PROVIDER---------------------------------------

PROVIDER_CmPlugNode_Demo& SERVICE_CmPlugNode_Demo::Provider()
{
  // Instantiate a PROVIDER class if it does not exist yet
  if( NULL==ServiceProvider )
  {
    LocalProvider = new PROVIDER_CmPlugNode_Demo(NULL, ServiceUURI.getString().getBuffer());
    ServiceProvider = LocalProvider;

		if (NULL != LocalProvider){
			LOG5("ID=", ContactID, " attached ID=", LocalProvider->getContactID(), " PROVIDER_CmPlugNode_Demo", Msg, CMLOG_PlugNodes)
		}
	}

  // Throw exception in case of memory problems
  if( NULL==ServiceProvider )
    throw(0);

	return *(PROVIDER_CmPlugNode_Demo *)ServiceProvider;
}

//------------------------------------------------------------------------------
// PROVIDER_CmPlugNode_Demo
//------------------------------------------------------------------------------

PROVIDER_CmPlugNode_Demo::PROVIDER_CmPlugNode_Demo(const char *_UURI, const char *_RootUURI)
	: CmPlugNode(_UURI, _RootUURI)
{
	LOG3("ID=", ContactID, " new PROVIDER_CmPlugNode_Demo ", Msg, CMLOG_PlugNodes)
}

PROVIDER_CmPlugNode_Demo::~PROVIDER_CmPlugNode_Demo()
{
	LOG3("ID=", ContactID, " del PROVIDER_CmPlugNode_Demo", Msg, CMLOG_Resource)
}

CmUURI& PROVIDER_CmPlugNode_Demo::getUURI()
{
  // Actual PROVIDER implementation of the 'getUURI()' SERVICE function
  return ServiceUURI;
}

bool PROVIDER_CmPlugNode_Demo::processInformation(CmString& _Information)
{
	// check if this is the intended recipient of the information
	CmString Recipient;
	Recipient.assignSubString(getUURI().getString().getLength() - 1, &_Information);
	if (Recipient != getUURI().getString()) return false;

	// check for query token
	if (_Information[Recipient.getLength() + 1] != '`') return false;

	// The provider is supposed to process information and to send a response,
	// e.g. just save it locally for later use
	LastInformation.assignSubString(_Information.getLength(), &_Information, Recipient.getLength() + 3);

	LOG5("ID=", ContactID, " CmPlugNode_Demo::processInformation() '", LastInformation, "'", Msg, CMLOG_Info)

	return true;
}

const CmString& PROVIDER_CmPlugNode_Demo::getLastInfo()
{
	return LastInformation;
}


