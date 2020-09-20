//////////////////////////////////////////////////////////////////////////////
//
// CmGateway.cpp: Implementation of a CmGateway class
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

#include "Services/PlugNode/CmGateway.h"
#include "Services/PlugNode/CmGatewayLAN.h"

// connection lists 
CmGatewayConnection* GatewayConnections = NULL;
CmParallelFTL GatewayConnectionLock;

//----------------------------------------------------------------------------
// SERVICE_CmGateway class
//----------------------------------------------------------------------------
SERVICE_CmGateway::SERVICE_CmGateway(const CmConnectionInfo& _ConnectionInfo)
// Initialize SERVICE UURI (=interface)
: CmPlugNode(UURI_SERVICE_CmGateway)
{
	ServerConnectionInfo = _ConnectionInfo;

	// Initialize
	LocalProvider = NULL;
	isThreadShutdown = false;
	isThreadFinished = false;
	isShutdownGateway = false;
	isFTLight = true;

	LOG3("ID=", ContactID, " new CmGateway ", Msg, CMLOG_Network)
}

SERVICE_CmGateway::~SERVICE_CmGateway()
{
	LOG3("ID=", ContactID, " del CmGateway", Msg, CMLOG_Network)

	// stop listening thread before removing provider
	isShutdownGateway = true;
	stopParallel();

  // Delete a local SERVICE implementation (PROVIDER) if it exists
  if( NULL!=LocalProvider )
    delete LocalProvider;
}

//------Service-functions-----------------------------------------------------

const CmUURI& SERVICE_CmGateway::getProviderUURI()
{
  return Provider().getUURI();
}

bool SERVICE_CmGateway::setNetworkUURI(const CmUURI& _NetworkUURI)
{
	// Set network UURI in the CmServiceAccess base class
	CmServiceAccess::setNetworkUURI(_NetworkUURI);

	return true;
}

bool SERVICE_CmGateway::joinNetwork(const CmUURI& _NetworkUURI, CmConnectionInfo *_ConnectionInfo)
{
	// verify destination parameters
	if ((NULL == _ConnectionInfo) || (_NetworkUURI != getNetworkUURI()) || (ServerConnectionInfo.ConnectionType != _ConnectionInfo->ConnectionType)) return false;

	// save remote gateway connection info
	RemoteGatewayConnectionInfo = *_ConnectionInfo;

	// establish a connection to a peer
	return Provider().joinNetwork(NetworkUURI, _ConnectionInfo);
}

bool SERVICE_CmGateway::findGateway(const CmUURI& _ServiceUURI, CmConnectionInfo& _GatewayInfo)
{
	// remember recipient UURI;
	RecipientUURI = _ServiceUURI;

	// preset known remote gateway connection info
	_GatewayInfo = RemoteGatewayConnectionInfo;

	return Provider().findGateway(_ServiceUURI, _GatewayInfo);
}

bool SERVICE_CmGateway::setConnectionInfo(CmConnectionInfo& _GatewayInfo)
{
	// update connection info inside a NetworkUURI
	RemoteGatewayConnectionInfo = _GatewayInfo;

	return true;
}

bool SERVICE_CmGateway::sendInfo(const CmString& _Info, const CmUURI& _ServiceUURI, bool _isControl, bool _isFTLight, CmString* _RecipientUURI)
{
	// find a gateway for ServiceUURI
	CmConnectionInfo GatewayConnectionInfo;
	if (false == findGateway(_ServiceUURI, GatewayConnectionInfo)){
		LOG5("ID=", ContactID, " no gateway found for '", _ServiceUURI.getText(), "'", Msg, CMLOG_Network)
		return false;
	}

	LOG4("ID=", ContactID, " CmGateway::send info to host=", CmString::IP2String(ServerConnectionInfo.LAN.DstHost, ServerConnectionInfo.LAN.DstPort), Msg, CMLOG_Network)
		
	// add ServiceUURI to _Info
	CmString Info;
	isFTLight = _isFTLight;
	if (isFTLight){
		Info = _ServiceUURI.getText();
		Info += _isControl ? "," : ",`,";
		Info += _Info;
	}
	else{
		Info = _Info;
		// save a recipient UURI if available
		_RecipientUURI != NULL ? RecipientUURI = *_RecipientUURI : RecipientUURI = "";
	}

	LOG5("ID=", ContactID, " CmGateway::send info '", Info, "'", Msg1, CMLOG_Network)

	// send info to remote gateway
	if (false == sendInfoToGateway(Info, GatewayConnectionInfo)) return false;

	return true;
}

bool SERVICE_CmGateway::sendInfoToGateway(CmString& _Info, CmConnectionInfo& _GatewayInfo)
{
	return Provider().sendInfoToGateway(_Info, _GatewayInfo);
}

bool SERVICE_CmGateway::processInformation(CmString& _Information)
{
	LOG5("ID=", ContactID, " CmGateway::processInformation() '", _Information, "'", Msg, CMLOG_Network)

	// get recipient UURI from _Information
	if (isFTLight){
		if (false == CmStringFTL::getIdentifier(_Information, RecipientUURI)) return false;
	}

	// find recipient for information
	CmServiceAccess *Recipient = findServiceUURI(RecipientUURI);
	if (NULL == Recipient) return false;

	// forward information to addressed recipient
	Recipient->processInformation(_Information);

	return true;
}

//------Service-access-through-a-PROVIDER-------------------------------------

SERVICE_CmGateway& SERVICE_CmGateway::Provider()
{
	enterSerialize(1);

	// Look-up or instantiate a CmGateway PROVIDER class if it is not a valid local provider
	if ((NULL == ServiceNetworkGateway) || (LocalProvider != ServiceNetworkGateway))
	{
		// check if requested provider exists already in this address space
		CmUURI ProviderUURI(UURI_SERVICE_CmGateway, CM_UURI_ROOT_COSMOS);
		switch (ServerConnectionInfo.ConnectionType){
		case CMCONNECTION_TYPE_TCP: ProviderUURI += UURI_PROVIDER_CmGatewayTCP; break;

		default: break;
		}
		// connect to existing provider
		ServiceNetworkGateway = (SERVICE_CmGateway*)findServiceUURI(ProviderUURI);

		if (NULL == ServiceNetworkGateway){
			// Generate a new provider according to connection type
			switch (ServerConnectionInfo.ConnectionType){
			case CMCONNECTION_TYPE_TCP: ServiceNetworkGateway = new PROVIDER_CmGatewayTCP(ServerConnectionInfo, NetworkUURI, ProviderUURI); break;

			default: break;
			}
			LocalProvider = ServiceNetworkGateway;
		}

		LOG6("ID=", ContactID, " attached ID=", ServiceNetworkGateway->ContactID, " CmGateway___ NetworkUURI=", ServiceNetworkGateway->getNetworkUURI().getText(), Msg, CMLOG_Network)
	}

	leaveSerialize();

	// Throw exception in case of memory problems
	if (NULL == ServiceNetworkGateway)
		throw(0);

	return *ServiceNetworkGateway;
}

//----------------------------------------------------------------------------
// CmGatewayConnection class
//----------------------------------------------------------------------------
CmGatewayConnection::CmGatewayConnection()
{
	// initialize
	NextGatewayConnection = NULL;

	LOG4("ID=", CmString::UInt2Hex((uint64)ThreadID), " new CmGatewayConnection ADDR=", CmString::UInt2Hex((uint64)this), Msg1, CMLOG_Network)
}
CmGatewayConnection::~CmGatewayConnection()
{
	LOG4("ID=", CmString::UInt2Hex((uint64)ThreadID), " del CmGatewayConnection ADDR=", CmString::UInt2Hex((uint64)this), Msg1, CMLOG_Network)
}

bool CmGatewayConnection::addToGatewayConnectionList()
{
	// remove idle connections
	if (false == cleanupGatewayConnections()) return false;

	GatewayConnectionLock.enterSerialize(2);

	// add new member to list
	NextGatewayConnection = GatewayConnections;
	GatewayConnections = this;

	GatewayConnectionLock.leaveSerialize();

	return true;
}

bool CmGatewayConnection::cleanupGatewayConnections()
{
	GatewayConnectionLock.enterSerialize(3);

	// check whether connections have become idle and can be removed
	CmGatewayConnection **Connection = &GatewayConnections;
	while (NULL != *Connection){
		if (false == (*Connection)->isRunningParallel()){
			CmGatewayConnection *DeleteConnection = *Connection;
			*Connection = DeleteConnection->NextGatewayConnection;
			delete DeleteConnection;
		}
		else{
			Connection = &(*Connection)->NextGatewayConnection;
		}
	}

	GatewayConnectionLock.leaveSerialize();

	return true;
}

bool CmGatewayConnection::isMatchingGatewayConnection(CmConnectionInfo& /*_GatewayInfo*/)
{
	// this function has to be overwritten by derived classes
	return false;
}

bool CmGatewayConnection::sendInfoToGateway(CmString& /*_Info*/)
{
	// this function has to be overwritten by derived classes
	return false;
}

