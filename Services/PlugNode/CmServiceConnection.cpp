//////////////////////////////////////////////////////////////////////////////
//
// CmServiceConnection.cpp: Implementation of the CmServiceConnection classes
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

#include "Services/PlugNode/CmServiceConnection.h"
#include "FTLight/CmStringFTL.h"

using namespace Cosmos;

//------------------------------------------------------------------------------
// CmServiceConnection class
//------------------------------------------------------------------------------

CmServiceConnection::CmServiceConnection()
{
  // Initialize workspace
	ConnectionID = CONN_NONE;
	ConnectionState = STATE_IDLE;
	CommunicationReceiver = NULL;
	RxInfo = NULL;
	TxInfo = new CmCommunication;
  NextReadPosition = 0;  // first readLine() position in Rx stream
  NextWritePosition = 0; // first writeLine() position in Tx stream;
	Peer = NULL;
	CommandX = NULL;
	PeerInfo.ConnectionType = CMCONNECTION_TYPE_ADR;
	PeerInfo.ADR.ContactAddress = NULL;
	PeerInfo.ADR.ContactID = -1;
}

CmServiceConnection::~CmServiceConnection()
{
	LOG4("ID=", ConnectionInfo.ADR.ContactID, " del CmServiceConnection ADDR=", CmString::UInt2Hex((uint64)this), Msg, CMLOG_Resource)

  // cleanup
	if (NULL != TxInfo){
		delete TxInfo;
	}
	if (NULL != CommandX){
		CmString::releaseMemory<CmStringFTL>(CommandX, 1, isCmStringFTL);
	}
}

void CmServiceConnection::initServiceConnection(const CmUURI *_ServiceUURI, CmConnectionInfo *_ConnectionInfo)
{
	// Store a copy of the SERVICE UURI
	ServiceUURI = _ServiceUURI;

	// Save connection information
	ConnectionInfo.ConnectionType = CMCONNECTION_TYPE_ADR;
	ConnectionInfo.ADR.ContactAddress = NULL;
	ConnectionInfo.ADR.ContactID = -1;
	if (NULL != _ConnectionInfo)
		ConnectionInfo = *_ConnectionInfo;

	LOG4("ID=", ConnectionInfo.ADR.ContactID, " new CmServiceConnection ADDR=", CmString::UInt2Hex((uint64)this), Msg, CMLOG_Connection)
}

bool CmServiceConnection::sendCommand(CMCOMMAND _Command)
{
	return sendCommand(_Command, NULL, NULL, NULL);
}
bool CmServiceConnection::sendCommand(CMCOMMAND _Command, CmServiceConnection *_ServiceConnection1)
{
	return sendCommand(_Command, _ServiceConnection1, NULL, NULL);
}
bool CmServiceConnection::sendCommand(CMCOMMAND _Command, CmServiceConnection *_ServiceConnection1, CmServiceConnection *_ServiceConnection2)
{
	return sendCommand(_Command, _ServiceConnection1, _ServiceConnection2, NULL);
}
bool CmServiceConnection::sendCommand(CMCOMMAND _Command, CmServiceConnection *_ServiceConnection1, CmServiceConnection *_ServiceConnection2, CmServiceConnection *_ServiceConnection3)
{
	// generate a command token
	int32 Token = BINX_TOKEN;
	CmdWriter()->setLength(sizeof(Token));
	CmdWriter()->setAt(0, Token);
	
	// add sequence number and command code
	int64 SequenceNumber = CmdWriter()->incrementSequenceNumber();
	CmdWriter()->addItemX(SequenceNumber);
	CmdWriter()->addItemX(_Command);

	// add connection1/2/3 parameters
  CmConnectionType Type;
  void* Address;
  uint64 Port;
  CmServiceConnection *ServCon[] = { _ServiceConnection1, _ServiceConnection2, _ServiceConnection3 };
	for (int i = 0; i < 3; i++){

		// initialize for empty connection
		Type = CMCONNECTION_TYPE_NONE;
		Address = NULL;
		Port = 0;

		// get parameters for a valid connection
		if (NULL != ServCon[i]){
			Type = ServCon[i]->PeerInfo.ConnectionType;
			if (CMCONNECTION_TYPE_ADR == Type){
				Address = ServCon[i]->PeerInfo.ADR.ContactAddress;
				Port = ServCon[i]->PeerInfo.ADR.ContactID;
			}
			if (CMCONNECTION_TYPE_TCP == Type){
				Address = (PVOID)(uint64)ServCon[i]->PeerInfo.LAN.DstHost;
				Port = ServCon[i]->PeerInfo.LAN.DstPort;
			}
			if (CMCONNECTION_TYPE_UDP == Type){
				Address = (PVOID)(uint64)ServCon[i]->PeerInfo.LAN.DstHost;
				Port = ServCon[i]->PeerInfo.LAN.DstPort;
			}
		}

		// add parameters to output message
		CmdWriter()->addItemX(Type);
		CmdWriter()->addItemX(Address);
		CmdWriter()->addItemX(Port);
	}

	// switch state according to sent command if a unique state will be reached
	switch (_Command){
	case CMCOMMAND_START: break;
	case CMCOMMAND_CHECKIN: ConnectionState = STATE_CHECKIN; break;
	case CMCOMMAND_CONNECT: ConnectionState = STATE_CONNECT; break;
	case CMCOMMAND_CONFIRM: ConnectionState = STATE_CONFIRM; break;
	case CMCOMMAND_SWITCH: ConnectionState = STATE_IDLE; break;
	case CMCOMMAND_CONTACT: break;
	case CMCOMMAND_DISCONNECT: 
		if (CMCONNECTION_TYPE_ADR == PeerInfo.ConnectionType){
			PeerInfo.ADR.ContactAddress = NULL;
			PeerInfo.ADR.ContactID = -1;
		}
		break;
	default: break;
	}

	// send command token, record number and command code
	if (false == sendMessage(*CmdWriter())) return false;

	return true;
}

bool CmServiceConnection::sendMessage(CmString& Msg)
{
	// check writing stream
	if (NULL == TxInfo) return false;

	// Write a line to outgoing (Tx) stream buffer
	if (false == TxInfo->writeLine(Msg)) return false;

	return true;
}

bool CmServiceConnection::maintainNetwork(CmString& /*_Request*/)
{
	return true;
}

bool CmServiceConnection::switchToIdle()
{
	// switching to IDLE can only be done from another than IDLE state
	if (STATE_IDLE == ConnectionState ) return false;

	setState(STATE_IDLE);

	// disconnect from remote stream
	if (NULL != RxInfo){
		RxInfo->unregisterCommunicationReceiver(CommunicationReceiver, this);
		RxInfo = NULL;
		CommunicationReceiver = NULL;
	}

	// reset peer contact ID
	if (CMCONNECTION_TYPE_ADR == PeerInfo.ConnectionType){
		PeerInfo.ADR.ContactID = -1;
	}

	return true;
}

CMCONN CmServiceConnection::getConnectionID()
{
	return ConnectionID;
}

CmStream * CmServiceConnection::isPeerServiceUURI(const CmUURI& _ServiceUURI)
{
  // Check peer's SERVICE UURI against a specified UURI.
  if( NULL==Peer || NULL==Peer->ServiceUURI || (_ServiceUURI != *(Peer->ServiceUURI)) )
    return NULL;

	return RxInfo;
}

void CmServiceConnection::openConnection(const CmUURI& _NetworkUURI, CmServiceConnection *_PeerConnection, const CmConnectionInfo& _PeerContactInfo)
{
  // Check parameters
  if( NULL==_PeerConnection || 0==_NetworkUURI.getString().getLength() )
    return;

	// Remember peer's contact information
	PeerInfo = _PeerContactInfo;

  // Check streams (a valid Tx has to be available, a Rx must not exist yet)
	if (NULL == TxInfo || NULL != RxInfo)
    return;

  // Cross-connect service connections
  _PeerConnection->Peer = this;
  this->Peer = _PeerConnection;

  // Cross-connect Rx/Tx communication/streams
	_PeerConnection->RxInfo = TxInfo;
	RxInfo = _PeerConnection->TxInfo;
}

void CmServiceConnection::setPeerInfo(const CmConnectionInfo& _PeerInfo)
{
	PeerInfo = _PeerInfo;
}

CmServiceAccess * CmServiceConnection::getPeerContactAddress()
{
	// a connection type different from ADR has no valid ContactAddress
	if (CMCONNECTION_TYPE_ADR != PeerInfo.ConnectionType) return NULL;

	return PeerInfo.ADR.ContactAddress;
}

int64 CmServiceConnection::getPeerContactID()
{
	// a connection type different from ADR has no valid ContactID
	if (CMCONNECTION_TYPE_ADR != PeerInfo.ConnectionType) 
		return -1;

	return PeerInfo.ADR.ContactID;
}

void CmServiceConnection::setState(CMSTATE _ConnectionState)
{
	ConnectionState = _ConnectionState;
}

CMSTATE CmServiceConnection::getState()
{
	return ConnectionState;
}

CmString CmServiceConnection::reportState()
{
	CmString State;

	switch (ConnectionState){
	case STATE_IDLE: State = "IDLE"; break;
	case STATE_CHECKIN: State = "CHK"; break;
	case STATE_CONNECT: State = "CON"; break;
	case STATE_DISCONNECT: State = "DSC"; break;
	case STATE_CONFIRM: State = "CFM"; break;
	default: State = "<na>"; break;
	}

	return State;
}

void CmServiceConnection::disconnect()
{
	// a disconnect requires either state CONFIRM or DISCONNECT
	if (STATE_CONFIRM != ConnectionState && STATE_DISCONNECT != ConnectionState) return;

	// check if the DISCONNECT has to be confirmed
	if (STATE_DISCONNECT != ConnectionState){
		sendCommand(CMCOMMAND_DISCONNECT);
	}

	// disconnect from remote stream
	if (NULL != RxInfo){
		RxInfo->unregisterCommunicationReceiver(CommunicationReceiver, this);
		RxInfo = NULL;
		CommunicationReceiver = NULL;
	}

	// new state is now IDLE
	ConnectionState = STATE_IDLE;
	if (CMCONNECTION_TYPE_ADR == PeerInfo.ConnectionType){
		PeerInfo.ADR.ContactAddress = NULL;
		PeerInfo.ADR.ContactID = -1;
	}
}

bool CmServiceConnection::registerCommunicationReceiver(CmCommunication* _CommunicationReceiver, CMCONN _ConnectionID)
{
	// check conditions to register a CommunicationReceiver
	if (NULL == RxInfo)	return false;
	if (CONN_NONE != ConnectionID && _ConnectionID != ConnectionID) return false;
	if (STATE_IDLE != ConnectionState) return false;

	// assign a ConnectionID (of CmServiceAccess) to this service connection
	ConnectionID = _ConnectionID;
	CommunicationReceiver = _CommunicationReceiver;

	return RxInfo->registerCommunicationReceiver(_CommunicationReceiver, this);
}

bool CmServiceConnection::unregisterCommunicationReceiver(CmCommunication* _CommunicationReceiver, CMCONN _ConnectionID)
{
	// check conditions to unregister a CommunicationReceiver
	if (NULL == RxInfo)	return false;
	if (_ConnectionID != ConnectionID) return false;

	return RxInfo->unregisterCommunicationReceiver(_CommunicationReceiver, this);
}

CmStringFTL * CmServiceConnection::CmdWriter()
{
	// Check if a command writer has to be instantiated
	if (NULL == CommandX)
	{
		CommandX = CmString::allocateMemory<CmStringFTL>(1, isCmStringFTL);

		// Throw exception in case of memory problems
		if (NULL == CommandX)
			throw(0);
	}

	return CommandX;
}

//--------conversion-functions------------------------------------------------

uint32 CmServiceConnection::ip2host(CmString _IP)
{
	int32 Host = 0;
	CmLString *Addr = NULL;
	if (4 == _IP.Match(&Addr, "(\\d+).(\\d+).(\\d+).(\\d+)")){
		for (int32 i = 0; i < 4; i++){
			Host *= 256;
			Host += (int32)(*Addr)[i].getNumAsUint64();
		}
	}

	return Host;
}
CmString CmServiceConnection::host2ip(uint32 /*_Host*/)
{
	CmString IP;

	// ToDo: convert Host into an IP string

	return IP;
}


//------------------------------------------------------------------------------
// CmNetworkConnection class
//------------------------------------------------------------------------------

CmNetworkConnection::CmNetworkConnection(const CmUURI *_ServiceUURI, CmConnectionInfo *_ConnectionInfo)
 : CmServiceConnection()
{
	initServiceConnection(_ServiceUURI, _ConnectionInfo);
}

CmNetworkConnection::~CmNetworkConnection()
{

}

//------------------------------------------------------------------------------
// CmLocalConnection class
//------------------------------------------------------------------------------

CmLocalConnection::CmLocalConnection(const CmUURI *_ServiceUURI)
 : CmServiceConnection()
{
	initServiceConnection(_ServiceUURI);
}

CmLocalConnection::~CmLocalConnection()
{

}


