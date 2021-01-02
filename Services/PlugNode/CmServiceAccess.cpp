//////////////////////////////////////////////////////////////////////////////
//
// CmServiceAccess.cpp: Implementation of the CmServiceAccess classes
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

#include "Services/PlugNode/CmServiceAccess.h"
#include "Services/PlugNode/CmGateway.h"
#include "FTLight/CmStringFTL.h"

using namespace Cosmos;

// ContactPlugNode: a PlugNode known to others for logging into to a local network
extern class CmPlugNode *ContactPlugNode;

// ToDo: serialize network search/maintenance
CmParallelFTL ContactLock; 

// A list of CmServiceConnections that are scheduled for shut-down
CmServiceConnection *ShutDownConnections = NULL;
CmParallelFTL ServiceConnectionLock;

int32 ServiceCreated = 0;
int32 ServiceDeleted = 0;
int32 ServiceRemaining = 0;
int32 SwitchConnectionCreated = 0;
int32 OpenConnectionCreated = 0;
int32 ConnectionCreated = 0;
int32 ConnectionDeleted = 0;
int32 ConnectionRemaining = 0;

//------------------------------------------------------------------------------
// CmServiceAccess class
//------------------------------------------------------------------------------

CmServiceAccess::CmServiceAccess()
{
	ServiceCreated++;

  // Initialize state and local workspace
  ServiceUURI = NULL;
  Connection1 = NULL;
  Connection2 = NULL;
	Connection3 = NULL;
	Connection4 = NULL;
	ConnectionSwitch = NULL;
  ServiceNetworkGateway = NULL;
	CommandX = NULL;
	MessageX = NULL;
	Peers = PEERS_0;
	ContactInfo.ConnectionType = CMCONNECTION_TYPE_ADR;
	ContactInfo.ADR.ContactAddress = this;
	ContactInfo.ADR.ContactID = ContactID;

  // Initialize a loop list of service networks
  NetworkGateways = this;

	// initialize recent info network/connection
	RecentInfoServiceNetwork = NetworkGateways;
	RecentInfoConnection = CONN_NONE;
	SearchToken = 0;
}
CmServiceAccess::~CmServiceAccess()
{
	ServiceDeleted++;
	ServiceRemaining = ServiceCreated - ServiceDeleted;

  disconnectFromPeers();

	// schedule all connections for shutdwon
	shutdownConnections();

  // Shutdown and delete all service networks of this PlugNode
  CmServiceAccess *ShutdownNetwork;
	while (this != NetworkGateways)
  {
		ShutdownNetwork = NetworkGateways;
    NetworkGateways = NetworkGateways->NetworkGateways;
		// isolate shutdown network from network list
		ShutdownNetwork->NetworkGateways = ShutdownNetwork;
		delete ShutdownNetwork;
  }

	// delete command and message reader
	if (CommandX != NULL){
		CmString::releaseMemory<CmStringFTL>(CommandX, 1, isCmStringFTL);
	}
	if (MessageX != NULL){
		CmString::releaseMemory<CmStringFTL>(MessageX, 1, isCmStringFTL);
	}
}

//-----local-network-maintenance-(protected)----------------------------------

void CmServiceAccess::setServiceUURI(const CmUURI *_ServiceUURI)
{
	// Store a copy of the servcice UURI
	ServiceUURI = _ServiceUURI;
}
const CmUURI * CmServiceAccess::getServiceUURI()
{
	return ServiceUURI;
}

void CmServiceAccess::setNetworkUURI(const CmUURI& _NetworkUURI)
{
	NetworkUURI = _NetworkUURI;
}
const CmUURI& CmServiceAccess::getNetworkUURI()
{
	return NetworkUURI;
}

void CmServiceAccess::setContactID(int64 _ContactID)
{
	ContactID = _ContactID;

	// copy ContactID to ContactInfo in case of ADR type
	if (CMCONNECTION_TYPE_ADR == ContactInfo.ConnectionType){
		ContactInfo.ADR.ContactID = _ContactID;
	}
}

bool CmServiceAccess::contactServiceAccess()
{
	// run a contact attempt against each valid destination
	for (int32 Conn = CONN_1; Conn <= CONN_2; Conn++){

		char *CMCOMMAND_TYPE;
		char *CMCOMMAND_ADDRESS;
		char *CMCOMMAND_PORT;
		switch (Conn){
		case CONN_1:
			CMCOMMAND_TYPE = CMCOMMAND_TYPE1;
			CMCOMMAND_ADDRESS = CMCOMMAND_ADDRESS1;
			CMCOMMAND_PORT = CMCOMMAND_PORT1;
			break;
		case CONN_2:
			CMCOMMAND_TYPE = CMCOMMAND_TYPE2;
			CMCOMMAND_ADDRESS = CMCOMMAND_ADDRESS2;
			CMCOMMAND_PORT = CMCOMMAND_PORT2;
			break;
		default: return false;
		}

		// run contact attempt according to connection type
		// contact parameters will be obtained from recent network communication
		CmConnectionInfo ConnectionInfo;
		CmServiceAccess *ServiceAccess;
		uint32 HostAddress;
		uint16 Port;
		int32 Type;

		if (false == CmdReader()->getValue(CMCOMMAND_TYPE, Type)) return false;
		switch (Type){
		case CMCONNECTION_TYPE_ADR:
			// get contact address 
			if (false == CmdReader()->getValue(CMCOMMAND_ADDRESS, (void*&)ServiceAccess)) return false;

			// run contact attempt
			ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_ADR;
			ConnectionInfo.ADR.ContactAddress = this;
			ConnectionInfo.ADR.ContactID = ContactID;
			if (NULL != ServiceAccess && this != ServiceAccess){
				ServiceAccess->joinNetwork(CmUURI(), &ConnectionInfo);
			}
			break;
		case CMCONNECTION_TYPE_TCP:
			// get host address and port 
			if (false == CmdReader()->getValue(CMCOMMAND_ADDRESS, HostAddress)) return false;
			if (false == CmdReader()->getValue(CMCOMMAND_PORT, Port)) return false;
			// todo: run contact attempt

			break;
		case CMCONNECTION_TYPE_UDP:
			// get host address and port 
			if (false == CmdReader()->getValue(CMCOMMAND_ADDRESS, HostAddress)) return false;
			if (false == CmdReader()->getValue(CMCOMMAND_PORT, Port)) return false;
			// todo: run contact attempt

			break;
		default:
			continue;
		}
	}

	return true;
}

bool CmServiceAccess::switchServiceAccess()
{
	if ((NULL == ConnectionSwitch) || (NULL == Connection4)) return false;

	// send a SWITCH command to previous fourth service connection for current fourth service connection
	bool Ret = ConnectionSwitch->sendCommand(CMCOMMAND_SWITCH, Connection4);

	// schedule connection for shutdown at a later time
	ServiceConnectionLock.enterSerialize(CMLOCKID_CmServiceAccess_switchServiceAccess);
	ConnectionSwitch != NULL ? ConnectionSwitch->NextServiceConnection = ShutDownConnections : 0;
	ShutDownConnections = ConnectionSwitch;
	ConnectionSwitch = NULL;
	ServiceConnectionLock.leaveSerialize();

	// delete idle connections
	deleteIdleConnections();

	return Ret;
}

bool CmServiceAccess::processMessage(CmStream& _Message, CmServiceConnection *_ServiceConnection)
{
	// obtain a FTLight line
	CmString LineX;
	_Message.readLine(LineX);

	// check if information has become available
	if (0 == LineX.getLength()) return false;

	// check for network maintenance command
	if (BINX_TOKEN == *((uint32*)(LineX.getBinary())) && NULL != _ServiceConnection){
		CMCONN ConnectionID = _ServiceConnection->getConnectionID();
		bool isOffline;
		if(false == maintainNetwork(LineX, _ServiceConnection, ConnectionID, isOffline)) return false;
		if (isOffline){
			// NOTE: This does not work but crashes!
			//       The intention is to bring disconnected PlugNodes in to the network again.
			//// check whether all connections are idle
			//if (NULL != Connection1 && STATE_IDLE != Connection1->getState()) return true;
			//if (NULL != Connection2 && STATE_IDLE != Connection2->getState()) return true;
			//if (NULL != Connection3 && STATE_IDLE != Connection3->getState()) return true;
			//if (NULL != Connection4 && STATE_IDLE != Connection4->getState()) return true;
			//// encountered disconnect: try to login again to contact PlugNode
			//return reconnectNetwork();
		}
		return true;
	}

	// TEST
	CmString Info;
	getLastInfo(Info);
	Info += " - ";
	Info += _ServiceConnection->getConnectionID();
	Info += "/";
	Info += _ServiceConnection->getPeerContactID();
	Info += ":";
	Info += LineX;
	processInformation(Info);

	return true;
}

bool CmServiceAccess::maintainNetwork(CmString& _Request, CmServiceConnection *_ServiceConnection, CMCONN /*_ConnectionID*/, bool& _isOffline)
{
	// check parameters
	if (NULL == _ServiceConnection) return false;
	// read command request
	CmdReader()->processStringFTL(_Request);
	// get sequence number and command code
	int64 SeqNumber;
	uint32 Command;
	if (false == CmdReader()->getValue(CMCOMMAND_SEQNUM, SeqNumber)) return false;
	if (false == CmdReader()->getValue(CMCOMMAND_COMMAND, Command)) return false;

	//Process next message dependent on current state
	_isOffline = false;
	switch (Peers){
	case PEERS_0:
		switch (Command){
		case CMCOMMAND_START:
			_ServiceConnection->sendCommand(CMCOMMAND_CHECKIN);
			break;
		case CMCOMMAND_CHECKIN:
			_ServiceConnection->sendCommand(CMCOMMAND_CONNECT);
			break;
		case CMCOMMAND_CONNECT:
			Peers = PEERS_1;
			_ServiceConnection->sendCommand(CMCOMMAND_CONFIRM);
			break;
		case CMCOMMAND_CONFIRM:
			Peers = PEERS_1;
			_ServiceConnection->setState(STATE_CONFIRM);
			break;
		case CMCOMMAND_SWITCH:
			break;
		case CMCOMMAND_CONTACT:
			contactServiceAccess();
			break;
		case CMCOMMAND_DISCONNECT:
			_ServiceConnection->disconnect();
			_isOffline = true;
			break;
		}
		break;
	case PEERS_1:
		switch (Command){
			case CMCOMMAND_START:
		break;
			case CMCOMMAND_CHECKIN:
				_ServiceConnection->sendCommand(CMCOMMAND_CONNECT);
				break;
		case CMCOMMAND_CONNECT:
			incrementPeers();
			_ServiceConnection->sendCommand(CMCOMMAND_CONFIRM);
			break;
		case CMCOMMAND_CONFIRM:
			incrementPeers();
			_ServiceConnection->setState(STATE_CONFIRM);
			break;
		case CMCOMMAND_SWITCH:
			decrementPeers();
			_ServiceConnection->switchToIdle();
			contactServiceAccess();
			break;
		case CMCOMMAND_CONTACT:
			contactServiceAccess();
			break;
		case CMCOMMAND_DISCONNECT:
			decrementPeers();
			_ServiceConnection->disconnect();
			_isOffline = true;
			break;
		}
		break;
	case PEERS_2:
		switch (Command){
		case CMCOMMAND_START:
			break;
		case CMCOMMAND_CHECKIN:
			_ServiceConnection->sendCommand(CMCOMMAND_CONNECT);
			_ServiceConnection->sendCommand(CMCOMMAND_CONTACT, Connection2);
			break;
		case CMCOMMAND_CONNECT:
			incrementPeers();
			_ServiceConnection->sendCommand(CMCOMMAND_CONFIRM);
			break;
		case CMCOMMAND_CONFIRM:
			incrementPeers();
			_ServiceConnection->setState(STATE_CONFIRM);
			break;
		case CMCOMMAND_SWITCH:
			decrementPeers();
			_ServiceConnection->switchToIdle();
			contactServiceAccess();
			break;
		case CMCOMMAND_CONTACT:
			contactServiceAccess();
			break;
		case CMCOMMAND_DISCONNECT:
			decrementPeers();
			_ServiceConnection->disconnect();
			break;
		}
		break;
	case PEERS_3:
		switch (Command){
		case CMCOMMAND_START:
			break;
		case CMCOMMAND_CHECKIN:
			_ServiceConnection->sendCommand(CMCOMMAND_CONNECT);
			_ServiceConnection->sendCommand(CMCOMMAND_CONTACT, Connection2);
			break;
		case CMCOMMAND_CONNECT:
			incrementPeers();
			_ServiceConnection->sendCommand(CMCOMMAND_CONFIRM);
			break;
		case CMCOMMAND_CONFIRM:
			incrementPeers();
			_ServiceConnection->setState(STATE_CONFIRM);
			break;
		case CMCOMMAND_SWITCH:
			decrementPeers();
			_ServiceConnection->switchToIdle();
			contactServiceAccess();
			break;
		case CMCOMMAND_CONTACT:
			contactServiceAccess();
			break;
		case CMCOMMAND_DISCONNECT:
			decrementPeers();
			_ServiceConnection->disconnect();
			break;
		}
		break;
	case PEERS_4:
		switch (Command){
		case CMCOMMAND_START:
			break;
		case CMCOMMAND_CHECKIN:
			_ServiceConnection->sendCommand(CMCOMMAND_CONNECT);
			_ServiceConnection->sendCommand(CMCOMMAND_CONTACT, Connection2); 
			break;
		case CMCOMMAND_CONNECT:
			_ServiceConnection->sendCommand(CMCOMMAND_CONFIRM);
			break;
		case CMCOMMAND_CONFIRM:
			_ServiceConnection->setState(STATE_CONFIRM);
			break;
		case CMCOMMAND_SWITCH:
			decrementPeers();
			_ServiceConnection->switchToIdle();
			contactServiceAccess();
			break;
		case CMCOMMAND_CONTACT:
			break;
		case CMCOMMAND_DISCONNECT:
			decrementPeers();
			_ServiceConnection->disconnect();
			break;
		}
		break;
	}

	return true;
}

bool CmServiceAccess::reconnectNetwork()
{
	// this virtual function will be overwritten by a PlugNode.
	return true;
}

void CmServiceAccess::sendMessage(CmString& _Message, CMCONN _Connection)
{
	// Select specified ServiceConnection
	CmServiceConnection *ServCon = NULL;

	switch (_Connection)
	{
	case CONN_1: ServCon = Connection1; break;
	case CONN_2: ServCon = Connection2; break;
	case CONN_3: ServCon = Connection3; break;
	case CONN_4: ServCon = Connection4; break;
	default: ServCon = NULL;
	}

	// Forward message to selected ServiceConnection
	if (NULL != ServCon){
		ServCon->sendMessage(_Message);
	}
}

CmStream *CmServiceAccess::getStream(const CmUURI& _ServiceUURI)
{
	CmStream *Stream = NULL;

  if( NULL==Stream && NULL!=Connection1 )
    Stream = Connection1->isPeerServiceUURI(_ServiceUURI);
  if( NULL==Stream && NULL!=Connection2 )
    Stream = Connection2->isPeerServiceUURI(_ServiceUURI);
	if (NULL == Stream && NULL != Connection3)
		Stream = Connection3->isPeerServiceUURI(_ServiceUURI);
	if (NULL == Stream && NULL != Connection4)
		Stream = Connection4->isPeerServiceUURI(_ServiceUURI);

  return Stream;
}

void CmServiceAccess::disconnectFromPeers()
{
  // Initiate a DISCONNECT for all service connections
  disconnect(CONN_1);
  disconnect(CONN_2);
	disconnect(CONN_3);
	disconnect(CONN_4);
}

void CmServiceAccess::disconnect(CMCONN _ConnectionID)
{
  // Select specified ServiceConnection
	CmServiceConnection *ServCon = NULL;
	switch (_ConnectionID)
  {
	case CONN_1: ServCon = Connection1; break;
	case CONN_2: ServCon = Connection2; break;
	case CONN_3: ServCon = Connection3; break;
	case CONN_4: ServCon = Connection4; break;
	default: ServCon = NULL;
  }

	// Disconnect from selected ServiceConnection
	if (NULL != ServCon && STATE_IDLE != ServCon->getState())
	{
		// switch temporarily from state CONFIRM to state DISCONNECT
		if (STATE_CONFIRM == ServCon->getState()){
			ServCon->setState(STATE_DISCONNECT);
			ServCon->sendCommand(CMCOMMAND_DISCONNECT);
		}
		ServCon->setState(STATE_DISCONNECT);
	}
}

bool CmServiceAccess::shutdownConnections()
{
	ServiceConnectionLock.enterSerialize(CMLOCKID_CmServiceAccess_shutdownConnections);

	// schedule all connections for shutdown
	CmServiceConnection *Connections[5] = { Connection1, Connection2, Connection3, Connection4, ConnectionSwitch };
	for (int32 i = 0; i < 5; i++){
		if (NULL != Connections[i]){
			Connections[i]->NextServiceConnection = ShutDownConnections;
			ShutDownConnections = Connections[i];
		}
	}

	ServiceConnectionLock.leaveSerialize();

	// delete idle connections
	return deleteIdleConnections();
}

bool CmServiceAccess::deleteIdleConnections()
{
	ServiceConnectionLock.enterSerialize(CMLOCKID_CmServiceAccess_deleteIdleConnections);

	CmServiceConnection **Connection = &ShutDownConnections;
	while (NULL != *Connection){
		// check if connection is already idle
		if ((*Connection)->getState() == STATE_IDLE){
			// remove idle connection
			CmServiceConnection *IdleConnection = *Connection;
			*Connection = (*Connection)->NextServiceConnection;
			CmString::releaseMemory<CmServiceConnection>(IdleConnection, 1, isCmServiceConnection);
			ConnectionDeleted++;
		}
		else{
			Connection = &(*Connection)->NextServiceConnection;
		}
	}

	// remaining connections
	ConnectionRemaining = ConnectionCreated - ConnectionDeleted;
	LOG8("ID=", ContactID, " Connections remaining/opened/switched: ", ConnectionRemaining, "/", OpenConnectionCreated, "/", SwitchConnectionCreated, Msg1, CMLOG_Network)

	ServiceConnectionLock.leaveSerialize();

	return true;
}

bool CmServiceAccess::deleteConnections()
{
	ServiceConnectionLock.enterSerialize(CMLOCKID_CmServiceAccess_deleteConnections);

	// delete remaining connections
	CmServiceConnection **Connection = &ShutDownConnections;
	while (NULL != *Connection){
		// remove a connection
		CmServiceConnection *DeleteConnection = *Connection;
		*Connection = (*Connection)->NextServiceConnection;
		CmString::releaseMemory<CmServiceConnection>(DeleteConnection, 1, isCmServiceConnection);
		ConnectionDeleted++;
		ConnectionRemaining = ConnectionCreated - ConnectionDeleted;
	}

	ServiceConnectionLock.leaveSerialize();

	return true;
}

CmServiceAccess * CmServiceAccess::findServiceUURI(const CmUURI& _ServiceUURI, uint64 _SearchToken)
{
	LOG4("ID=", ContactID, " findServiceUURI() UURI=", _ServiceUURI.getString(), Msg, CMLOG_Connection)

	CmString _UURI = _ServiceUURI.getString();
	CmString UURI = getServiceUURI()->getString();

	// check search token against previous search
	_SearchToken == 0 ? _SearchToken = CmDateTime::getSysClockNanoSec() : 0;
	if (_SearchToken == SearchToken) return NULL;
	SearchToken = _SearchToken;

	if (_ServiceUURI == *getServiceUURI()){
		return this;
	}

	// let the request run through network
	CmServiceAccess* ServiceAccess = NULL;
	CmServiceConnection* Connection[] = { Connection1, Connection2, Connection3, Connection4 };
	for (int i = 0; i < 4; i++){
		if (NULL == Connection[i]) continue;
		if (NULL == Connection[i]->getPeerContactAddress()) continue;
		if (NULL != (ServiceAccess = Connection[i]->getPeerContactAddress()->findServiceUURI(_ServiceUURI, SearchToken)))
			break;
	}
	return ServiceAccess;
}

bool CmServiceAccess::joinNetwork(const CmUURI& _NetworkUURI, CmConnectionInfo *_ConnectionInfo)
{
	LOG4("ID=", ContactID, " CmServiceAccess::joinNetwork() NetworkUURI=",_NetworkUURI.getText(), Msg, CMLOG_Connection)

  // Obtain/derive socket information
  CmConnectionInfo ConnectionInfo = {};
  if (NULL != _ConnectionInfo) 
    ConnectionInfo = *_ConnectionInfo;
  else
  {
    // Derive socket information from NetworkUURI
    if (_NetworkUURI.getString().isPrefix(CM_UURI_ROOT_COSMOS)){
      ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_ADR;
      ConnectionInfo.ADR.ContactAddress = ContactPlugNode;
			ConnectionInfo.ADR.ContactID = ContactPlugNode->getContactID();
    }
		else if (_NetworkUURI.getString().isPrefix(CM_NETWORK_UURI_LAN_TCP)){
			ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_TCP;
			ConnectionInfo.LAN.DstPort = CM_NETWORK_PORT_LAN_TCP;
			ConnectionInfo.LAN.DstHost = CM_SOCKET_DEFAULT_LOCALHOST;
		}	
		else if (_NetworkUURI.getString().isPrefix(CM_NETWORK_UURI_LAN_UDP)){
			ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_UDP;
			ConnectionInfo.LAN.DstPort = CM_NETWORK_PORT_LAN_UDP;
			ConnectionInfo.LAN.DstHost = CM_SOCKET_DEFAULT_LOCALHOST;
		}
		else if (_NetworkUURI.getString().isPrefix(CM_NETWORK_UURI_WEGALINK)){
			ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_TCP;
			ConnectionInfo.LAN.DstPort = CM_NETWORK_PORT_WEGALINK;
			ConnectionInfo.LAN.DstHost = CM_SOCKET_DEFAULT_LOCALHOST; // later (DNS): CM_NETWORK_HOST_WEGALINK;
		}
		else{
      ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_NONE;
			return false;
    }
  }

  // Check if a new network gateway is needed
	CmServiceAccess *NetworkGateway = NULL;
	if (CMCONNECTION_TYPE_ADR == ConnectionInfo.ConnectionType){
		// The "gateway" is a contact PlugNode's address delivered inside NetworkAddress struct
		NetworkGateway = ConnectionInfo.ADR.ContactAddress;
		NetworkGateway->setContactID(ConnectionInfo.ADR.ContactID);
	}else{
		// Check if provided NetworkUURI matches any of the connected network gateways
		NetworkGateway = this->NetworkGateways;
		while (NetworkGateway != this){
			if (NetworkGateway->getNetworkUURI() == _NetworkUURI) break;
			NetworkGateway = NetworkGateway->NetworkGateways;
		}

		// check if a suitable network gateway was found (this -> no)
		if (NetworkGateway == this){
			LOG4("ID=", ContactID, " add SERVICE_CmGateway NetworkUURI=", _NetworkUURI.getText(), Msg, CMLOG_Network)

			// Create a new service network access point
			NetworkGateway = new SERVICE_CmGateway(ConnectionInfo);

			// verify gateway and assign a network UURI
			if (NULL == NetworkGateway) return false;
			NetworkGateway->setNetworkUURI(_NetworkUURI);

			// Append new gateway to a list of service networks
			NetworkGateway->NetworkGateways = this->NetworkGateways;
			this->NetworkGateways = NetworkGateway;

			// Let the gateway login to desired service network
			if(false == NetworkGateway->joinNetwork((CmUURI)_NetworkUURI, &ConnectionInfo)) return false;

			// forward connection info
			NULL != _ConnectionInfo ? _ConnectionInfo->LAN.DstPort = ConnectionInfo.LAN.DstPort : 0;

			return true;
		}
		else{
			// update connection info for an existing NetworkUURI
			NetworkGateway->setConnectionInfo(ConnectionInfo);
		}
  }

	// reject if a service connection exists already
	CmServiceConnection *Connections[4] = { Connection1, Connection2, Connection3, Connection4 };
	for (int32 i = 0; i < 4; i++){
		if (NULL == Connections[i]) continue;
		// equal peer?
		if (ConnectionInfo.ADR.ContactAddress == Connections[i]->getPeerContactAddress())
			return false;
	}

  // Create a free ServiceConnection
	CmServiceConnection *Connection = NULL;
	CMCONN ConnectionID = CONN_NONE;
	if (NULL == Connection1 || STATE_IDLE == Connection1->getState()){
		if (NULL == Connection1){
			Connection1 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection1 ? Connection1->initServiceConnection(ServiceUURI, &ConnectionInfo) : 0;
			ConnectionCreated++;
		}
		ConnectionID = CONN_1;
		Connection = Connection1;
	}
	else
	if (NULL == Connection2 || STATE_IDLE == Connection2->getState()){
		if (NULL == Connection2){
			Connection2 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection2 ? Connection2->initServiceConnection(ServiceUURI, &ConnectionInfo) : 0;
			ConnectionCreated++;
		}
		ConnectionID = CONN_2;
		Connection = Connection2;
	}
	else
	if (NULL == Connection3 || STATE_IDLE == Connection3->getState()){
		if (NULL == Connection3){
			Connection3 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection3 ? Connection3->initServiceConnection(ServiceUURI, &ConnectionInfo) : 0;
			ConnectionCreated++;
		}
		ConnectionID = CONN_3;
		Connection = Connection3;
	}
	else
	if (NULL == Connection4 || STATE_IDLE == Connection4->getState()){
		if (NULL == Connection4){
			Connection4 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection4 ? Connection4->initServiceConnection(ServiceUURI, &ConnectionInfo) : 0;
			ConnectionCreated++;
		}
		ConnectionID = CONN_4;
		Connection = Connection4;
	}

	// check for connection switch
	if (NULL == Connection && NULL == ConnectionSwitch){
		// switch connection 4
		ConnectionSwitch = Connection4;
		Connection4 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
		NULL != Connection4 ? Connection4->initServiceConnection(ServiceUURI) : 0;
		ConnectionCreated++;
		SwitchConnectionCreated++;
		ConnectionID = CONN_4;
		Connection = Connection4;

		LOG3("ID=", ContactID, "SWITCH ", Msg1, CMLOG_Connection)
	}

  // Connect to desired service network
  if( NULL!=Connection )
  {
		// checkin to peer
		CmServiceConnection *PeerConnection;
		PeerConnection = NetworkGateway->openConnection(_NetworkUURI, Connection, ContactInfo);
		if (NULL == PeerConnection) return false;

		// register a communication callback
		Connection->registerCommunicationReceiver(this, ConnectionID);

		// remember peer's connection information
		Connection->setPeerInfo(NetworkGateway->ContactInfo);

		// check if a switch was initiated
		switchServiceAccess();

		// two-way connection established, have the peer check in to network
		PeerConnection->sendCommand(CMCOMMAND_CHECKIN);
	}
  else
    return false;

  return true;
}

bool CmServiceAccess::setConnectionInfo(CmConnectionInfo& /*_GatewayInfo*/)
{
	// this function is supposed to be overwritten by CmGateway

	return true;
}

CmServiceConnection * CmServiceAccess::openConnection(const CmUURI& _NetworkUURI, CmServiceConnection *_PeerConnection, const CmConnectionInfo& _PeerContactInfo)
{
  // Assign a local service connection
	CmServiceConnection *LocalConnection = NULL;
	CMCONN ConnectionID = CONN_NONE;
	if (NULL == Connection1 || STATE_IDLE == Connection1->getState()){
		if (NULL == Connection1){
			Connection1 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection1 ? Connection1->initServiceConnection(ServiceUURI) : 0;
			ConnectionCreated++;
			OpenConnectionCreated++;
		}
		ConnectionID = CONN_1;
		LocalConnection = Connection1;
	}
	else
	if (NULL == Connection2 || STATE_IDLE == Connection2->getState()){
		if (NULL == Connection2){
			Connection2 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection2 ? Connection2->initServiceConnection(ServiceUURI) : 0;
			ConnectionCreated++;
			OpenConnectionCreated++;
		}
		ConnectionID = CONN_2;
		LocalConnection = Connection2;
	}
	else
	if (NULL == Connection3 || STATE_IDLE == Connection3->getState()){
		if (NULL == Connection3){
			Connection3 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection3 ? Connection3->initServiceConnection(ServiceUURI) : 0;
			ConnectionCreated++;
			OpenConnectionCreated++;
		}
		ConnectionID = CONN_3;
		LocalConnection = Connection3;
	}
	else
	if (NULL == Connection4 || STATE_IDLE == Connection4->getState()){
		if (NULL == Connection4){
			Connection4 = CmString::allocateMemory<CmServiceConnection>(1, isCmServiceConnection);
			NULL != Connection4 ? Connection4->initServiceConnection(ServiceUURI) : 0;
			ConnectionCreated++;
			OpenConnectionCreated++;
		}
		ConnectionID = CONN_4;
		LocalConnection = Connection4;
	}

  // Establish a two-way service connection
	if (NULL != LocalConnection){
		LocalConnection->openConnection(_NetworkUURI, _PeerConnection, _PeerContactInfo);

		// register a communication callback
		if (false == LocalConnection->registerCommunicationReceiver(this, ConnectionID)) return NULL;
	}

  return LocalConnection;
}

void CmServiceAccess::reportState(CmString& _State, bool _isContactPlugNode)
{
	CmServiceAccess *PN = this;

	if (_isContactPlugNode){
		PN = ContactPlugNode;
	}

	// con-1
	_State = "        Con-1: ";
	if (NULL != PN->Connection1)
		_State += PN->Connection1->reportState();
	else
		_State += "NOC";
	// con-2
	_State += "     Con-2: ";
	if (NULL != PN->Connection2)
		_State += PN->Connection2->reportState();
	else
		_State += "NOC";
	// con-3
	_State += "     Con-3: ";
	if (NULL != PN->Connection3)
		_State += PN->Connection3->reportState();
	else
		_State += "NOC";
	// con-4
	_State += "     Con-4: ";
	if (NULL != PN->Connection4)
		_State += PN->Connection4->reportState();
	else
		_State += "NOC";
}

void CmServiceAccess::incrementPeers()
{
	switch(Peers){
	case PEERS_0: Peers = PEERS_1; break;
	case PEERS_1: Peers = PEERS_2; break;
	case PEERS_2: Peers = PEERS_3; break;
	case PEERS_3: Peers = PEERS_4; break;
	case PEERS_4: Peers = PEERS_4; break;
	default: Peers = Peers;
	}
}
void CmServiceAccess::decrementPeers()
{
	switch (Peers){
	case PEERS_0: Peers = PEERS_0; break;
	case PEERS_1: Peers = PEERS_0; break;
	case PEERS_2: Peers = PEERS_1; break;
	case PEERS_3: Peers = PEERS_2; break;
	case PEERS_4: Peers = PEERS_3; break;
	default: Peers = Peers;
	}
}
int32 CmServiceAccess::getPeers()
{
	switch (Peers){
	case PEERS_0: return 0;
	case PEERS_1: return 1;
	case PEERS_2: return 2;
	case PEERS_3: return 3;
	case PEERS_4: return 4;
	default: return 0;
	}
}

bool CmServiceAccess::sendInfo(const CmString& _Info, const CmUURI& _ServiceUURI, CMMODE _Mode, bool /*_isFTLight*/, CmString* /*_RecipientUURI*/)
{
	// find service UURI
	CmServiceAccess* Service = findServiceUURI(_ServiceUURI);
	if (NULL == Service) return false;

	// add destination _ServiceUURI as a predecessor to _Info if it is a FTLight info
	// add a query token in case it is not a control information
	CmString Info(_ServiceUURI.getText());
	Info += CM_QUERY == _Mode ? "," : ",`,";
	Info += _Info;

	LOG7("ID=", ContactID, " CmServiceAccess::sendInfo(ID", Service->ContactID, ") '", Info, "'", Msg, CMLOG_Info)

	// initiate information processing by peer
	return Service->processInformation(Info);
}
bool CmServiceAccess::sendInfo(const CmString& _Info, const CmUURI& _ServiceUURI, const CmUURI& _NetworkUURI, CMMODE _Mode, bool _isFTLight, CmString* _RecipientUURI)
{
	// check for local access
	if (_NetworkUURI == CM_UURI_ROOT_COSMOS){
		return sendInfo(_Info, _ServiceUURI, _Mode);
	}
	else{
		// find network UURI
		CmServiceAccess *Network = NetworkGateways;
		while (this != Network)
		{
			if (_NetworkUURI == Network->getNetworkUURI()){
				return Network->sendInfo(_Info, _ServiceUURI, _Mode, _isFTLight, _RecipientUURI);
			}
			Network = Network->NetworkGateways;
		}
	}

	// try to login to new network 
	if (false == joinNetwork(_NetworkUURI)) 
		return false;

	// wait until new network connection has logged in to peers
	Sleep(20);

	// try again to find service UURI at network UURI
	CmServiceAccess *Network = NetworkGateways;
	while (this != Network)
	{
		if (_NetworkUURI == Network->getNetworkUURI()){
			return Network->sendInfo(_Info, _ServiceUURI);
		}
		Network = Network->NetworkGateways;
	}

	return true;
}

//-----remote-network-maintenance-(private)-----------------------------------

bool CmServiceAccess::createNetworkGateway(const CmUURI& /*NetworkUURI*/)
{

	return true;
}

bool CmServiceAccess::queryNetworkForServiceUURI(const CmUURI& /*_ServiceUURI*/)
{

	return true;
}

CmServiceAccess * CmServiceAccess::findNetworkUURI(const CmUURI& _ServiceUURI)
{
	CmServiceAccess *Service = this;

	// Iterate through a list of all connected network services
	while (NULL != Service->ServiceUURI && _ServiceUURI != *Service->ServiceUURI)
	{
		// Loop end condition
		if (Service->NetworkGateways == this)
			break;

		// Step forward in a loop list of connected service networks
		Service = Service->NetworkGateways;
	}

	// Check if desired socket service could be found
	if ((NULL == Service->ServiceUURI) || (_ServiceUURI != *Service->ServiceUURI))
		return NULL;

	return Service;
}

//-----local-modules-(private)------------------------------------------------

CmStringFTL * CmServiceAccess::CmdReader()
{
	// Check if a command reader has to be instantiated
	if (NULL == CommandX)
	{
		CommandX = CmString::allocateMemory<CmStringFTL>(1, isCmStringFTL);

		// Throw exception in case of memory problems
		if (NULL == CommandX)
			throw(0);

		// initialize command reader
		CommandX->processStringFTL(CMCOMMAND_HEADER);
	}

	return CommandX;
}

CmStringFTL * CmServiceAccess::MsgReader()
{
	// Check if a message reader has to be instantiated
	if (NULL == MessageX)
	{
		MessageX = CmString::allocateMemory<CmStringFTL>(1, isCmStringFTL);

		// Throw exception in case of memory problems
		if (NULL == MessageX)
			throw(0);

		// initialize message reader
		MessageX->processStringFTL(CMMESSAGE_HEADER);
	}

	return MessageX;
}

int64	CmServiceAccess::getContactID(int32* _PeerCount, int64 *_ContactID1, int64 *_ContactID2, int64 *_ContactID3, int64 *_ContactID4, bool _isContactPlugNode)
{
	CmServiceAccess *PN = this;

	if (_isContactPlugNode){
		PN = ContactPlugNode;
	}

	// Determine peer count
	if (NULL != _PeerCount){
		switch (PN->getPeers()){
		case PEERS_0: *_PeerCount = 0; break;
		case PEERS_1: *_PeerCount = 1; break;
		case PEERS_2: *_PeerCount = 2; break;
		case PEERS_3: *_PeerCount = 3; break;
		case PEERS_4: *_PeerCount = 4; break;
		default: *_PeerCount = -1; break;
		}
	}

	// Query peers' contact IDs
	// peer 1 
	if (NULL != _ContactID1)
	{
		*_ContactID1 = -1;
		if (NULL != PN->Connection1)
			*_ContactID1 = PN->Connection1->getPeerContactID();
	}
	// peer 2
	if (NULL != _ContactID2)
	{
		*_ContactID2 = -1;
		if (NULL != PN->Connection2)
			*_ContactID2 = PN->Connection2->getPeerContactID();
	}
	// peer 3
	if (NULL != _ContactID3)
	{
		*_ContactID3 = -1;
		if (NULL != PN->Connection3)
			*_ContactID3 = PN->Connection3->getPeerContactID();
	}
	// peer 4
	if (NULL != _ContactID4)
	{
		*_ContactID4 = -1;
		if (NULL != PN->Connection4)
			*_ContactID4 = PN->Connection4->getPeerContactID();
	}

	// Contact PlugNode will report 0, others return their actual ContactID
	return PN->ContactID;
}




