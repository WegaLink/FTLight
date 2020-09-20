//////////////////////////////////////////////////////////////////////////////
//
// CmGatewayLAN.cpp: Implementation of a CmGatewayLAN class
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

#include "Services/PlugNode/CmGatewayLAN.h"

extern CmGatewayConnection* GatewayConnections;
extern CmParallelFTL GatewayConnectionLock;


//----------------------------------------------------------------------------
// PROVIDER_CmGatewayTCP
//----------------------------------------------------------------------------
PROVIDER_CmGatewayTCP::PROVIDER_CmGatewayTCP(const CmConnectionInfo& _ConnectionInfo, const CmUURI& _NetworkUURI, const CmUURI& _ServiceUURI)
	:SERVICE_CmGateway(_ConnectionInfo)
{
	LOG6("ID=", ContactID, " new PROVIDER_CmGatewayTCP port=", _ConnectionInfo.LAN.DstPort, " host=", CmString::UInt2Hex(_ConnectionInfo.LAN.DstHost), Msg, CMLOG_Network)

	// assign a network/service UURI
	NetworkUURI = _NetworkUURI;
	ServiceUURI = _ServiceUURI;

	// Initialize a TCP socket
	BindPort = ServerConnectionInfo.LAN.DstPort;
	Interface = ServerConnectionInfo.LAN.DstHost;
	server_socket = socket(AF_INET, SOCK_STREAM, ServerConnectionInfo.ConnectionType == CMCONNECTION_TYPE_UDP ? IPPROTO_UDP : IPPROTO_TCP);
	client_socket = INVALID_SOCKET;

	// Initialize a server Internet address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(BindPort);
	server_addr.sin_addr.S_un.S_addr = htonl(Interface);

	// Initialize a client Internet address
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = 0;
	client_addr.sin_addr.S_un.S_addr = 0;

	// Bind socket
	const int32 SocketRange = 14;	// try to bind on next available socket
	SetupReturn = SOCKET_ERROR;
	if (INVALID_SOCKET != server_socket)
	{
		for (int32 s = 0; s < SocketRange; s++){
			// try next port
			if (0 == (SetupReturn = bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)))) break;
			// failed, step to next port
			SetupReturn = WSAGetLastError();
			server_addr.sin_port = htons(++BindPort);
		}
	}
	else
		SetupReturn = WSAGetLastError();

	// start a listening thread if socket bind succeeded
	if (0 == SetupReturn){
		startParallel();
	}	else{
		if (INVALID_SOCKET != server_socket){
			// Close socket, free all resources
			closesocket(server_socket);
			server_socket = INVALID_SOCKET;
		}
	}
	LOG4("ID=", ContactID, " CmGatewayTCP listening on socket ", server_socket, Msg1, CMLOG_Network)
}

PROVIDER_CmGatewayTCP::~PROVIDER_CmGatewayTCP()
{
	LOG4("ID=", ContactID, " del CmGatewayTCP NetworkUURI=", getNetworkUURI().getText(), Msg, CMLOG_Resource)

	// shutdown background function
	stopParallel();

	// Check if a client socket exists
	if (INVALID_SOCKET != client_socket)
	{
		// Shutdown socket gracefully
		shutdown(client_socket, SD_BOTH);

		// Close socket, free all resources
		closesocket(client_socket);
	}

	// Check if a server socket exists
	if (INVALID_SOCKET != server_socket)
	{
		// Shutdown socket gracefully
		shutdown(server_socket, SD_BOTH);

		// Close socket, free all resources
		closesocket(server_socket);
	}

	// cleanup connection list
	CmGatewayConnection::cleanupGatewayConnections();
}

//------Service-functions-----------------CmGatewayTCP-PPROVIDER-----------

CmUURI& PROVIDER_CmGatewayTCP::getUURI()
{
	return ServiceUURI;
}

bool PROVIDER_CmGatewayTCP::joinNetwork(const CmUURI& /*_NetworkUURI*/, CmConnectionInfo *_ConnectionInfo)
{
	LOG6("ID=", ContactID, " join network on TCP port=", (NULL != _ConnectionInfo ? _ConnectionInfo->LAN.DstPort : 0), " host=", (NULL != _ConnectionInfo ? CmString::UInt2Hex(_ConnectionInfo->LAN.DstHost) : ""), Msg, CMLOG_Network)

	// ToDo: join network

	LOG4("ID=", ContactID, " joined network via TCP socket ", client_socket, Msg1, CMLOG_Network)
	return true;
}

bool PROVIDER_CmGatewayTCP::findGateway(const CmUURI& /*_ServiceUURI*/, CmConnectionInfo& /*_GatewayInfo*/)
{
	// ToDo: find gateway for given ServiceUURI

	return true;
}

bool PROVIDER_CmGatewayTCP::sendInfoToGateway(CmString& _Info, CmConnectionInfo& _GatewayInfo)
{
	GatewayConnectionLock.enterSerialize(4);

	// check whether a connection exists
	CmGatewayConnection *GatewayConnection = GatewayConnections;
	while (NULL != GatewayConnection){
		if (false != GatewayConnection->isRunningParallel()){
			// check whether connection matches
			if (GatewayConnection->isMatchingGatewayConnection(_GatewayInfo))
				break;
			// step to next connection
			GatewayConnection = GatewayConnection->NextGatewayConnection;
		}
		else{
			// skip inactive connection
			GatewayConnection = GatewayConnection->NextGatewayConnection;
		}
	}

	// send data if a connection was found
	if (NULL != GatewayConnection){
		if (GatewayConnection->sendInfoToGateway(_Info)) {
			// info was sent successfully
			GatewayConnectionLock.leaveSerialize();
			return true;
		}
	}

	GatewayConnectionLock.leaveSerialize();

	// try until a connection could be established successfully
	for (int Trials = 0;;Trials++){
		// create a new client socket 
		client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == client_socket)
			return false;

		// Initialize remote gateway address
		client_addr.sin_port = htons(_GatewayInfo.LAN.DstPort);
		client_addr.sin_addr.S_un.S_addr = htonl(_GatewayInfo.LAN.DstHost);

		// try to establish a connection to specified gateway
		u_long SocketMode = 1; // 1==non-blocking mode, 0==blocking mode
		if (SOCKET_ERROR == ioctlsocket(client_socket, FIONBIO, &SocketMode)){
			LOG4("ID=", ContactID, " failed to make socket non-blocking", client_socket, Msg1, CMLOG_Network)
		}
		if (SOCKET_ERROR == connect(client_socket, (SOCKADDR*)&client_addr, sizeof(client_addr))){
			// a non-blocking socket seems to always return SOCKET_ERROR
		}
		// wait for connect finishes
		fd_set FDSet;
		struct timeval tv;
		FD_ZERO(&FDSet);
		FD_SET(client_socket, &FDSet);
		tv.tv_sec = 0;  // timeout for waiting on socket connect
		tv.tv_usec = 20000;
		int32 RetSelect;
		if (0 == (RetSelect = select(0, NULL, &FDSet, NULL, &tv))){
			int SocketError = WSAGetLastError();
			LOG6("ID=", ContactID, " failed to open TCP socket (select error=", SocketError, ") IP=", CmString::IP2String(_GatewayInfo.LAN.DstHost, _GatewayInfo.LAN.DstPort), Msg1, CMLOG_Info)
			// Close socket, free all resources
			shutdown(client_socket, SD_BOTH);
			closesocket(client_socket);
			client_socket = INVALID_SOCKET;
			client_addr.sin_port = 0;
			client_addr.sin_addr.S_un.S_addr = 0;
			// give up after some trials
			const int32 MaxTrials = 50;
			if (Trials > MaxTrials){
				return false;
			}
			// try with a new socket
			continue;
		}
		break;
	}

	// switch socket back to blocking mode
	u_long SocketMode = 0; // 1==non-blocking mode, 0==blocking mode
	if (SOCKET_ERROR == ioctlsocket(client_socket, FIONBIO, &SocketMode)){
		LOG4("ID=", ContactID, " failed to make socket blocking", client_socket, Msg1, CMLOG_Network)
	}

	// operate new service connection in background
	const int32 LoopCount = 0;
	CmGatewayConnectionLAN* GatewayConnectionLAN = new CmGatewayConnectionLAN(this, client_socket);
	GatewayConnectionLAN->startParallel(LoopCount);
	GatewayConnectionLAN->addToGatewayConnectionList();

	// invalidate client socket for it will be maintained by CmGatewayConnectionLAN
	client_socket = INVALID_SOCKET;

	// Have info be sent by CmGatewayConnectionLAN
	if (false == GatewayConnectionLAN->sendInfoToGateway(_Info)) 
		return false;

	return true;
}

bool PROVIDER_CmGatewayTCP::runParallel()
{
	// make the server socket listening for incoming connections
	if (0 == SetupReturn){
		if (SOCKET_ERROR == listen(server_socket, 1)){
			SetupReturn = SOCKET_ERROR;
			LOG5("ID=", ContactID, " listening on socket ", server_socket, " failed", Msg, CMLOG_Network)
			return false;
		}
	}

	LOG4("ID=", ContactID, " listening on socket ", server_socket, Msg1, CMLOG_Network)

	// accept a new service connection
	SOCKET AcceptSocket;
	if (INVALID_SOCKET == (AcceptSocket = accept(server_socket, NULL, NULL))) 
		return false;
	if (isShutdownGateway) 
		return false;

	LOG5("ID=", ContactID, " accept connection on socket ", AcceptSocket, " (parallel)", Msg2, CMLOG_Network)

	// operate service connection in background
	const int32 LoopCount = 0;
	CmGatewayConnectionLAN* GatewayConnectionLAN = new CmGatewayConnectionLAN(this, AcceptSocket);
	GatewayConnectionLAN->startParallel(LoopCount);
	GatewayConnectionLAN->addToGatewayConnectionList();

	return true;
}


//----------------------------------------------------------------------------
// CmConnectionLAN
//----------------------------------------------------------------------------
CmGatewayConnectionLAN::CmGatewayConnectionLAN(SERVICE_CmGateway* _Gateway, SOCKET _ConnectionSocket)
{
	// save gateway and accept socket
	Gateway = _Gateway;
	ConnectionSocket = _ConnectionSocket;

	LOG6("ID=", (NULL != Gateway ? Gateway->getContactID() : 0), " new CmGatewayConnectionLAN ADDR=", CmString::UInt2Hex((uint64)this), " listening on socket ", ConnectionSocket, Msg1, CMLOG_Network)
}

CmGatewayConnectionLAN::~CmGatewayConnectionLAN()
{
	LOG6("ID=", (NULL != Gateway ? Gateway->getContactID() : 0), " shutdown CmGatewayConnectionLAN ADDR=", CmString::UInt2Hex((uint64)this), " on socket ", ConnectionSocket, Msg1, CMLOG_Resource)

	// shutdown data receiver thread
	stopParallel();
	// release socket
	shutdown(ConnectionSocket, SD_BOTH);
	closesocket(ConnectionSocket);
}

//------Service-functions-----------------CmConnectionLAN------------------

bool CmGatewayConnectionLAN::runParallel()
{
	// get data from peer in background
	if (false == receiveData()) return false;

	return true;
}

bool CmGatewayConnectionLAN::receiveData()
{
	int64 ContactID = NULL != Gateway ? Gateway->getContactID() : 0;
	LOG4("ID=", ContactID, " waiting for data on socket ", ConnectionSocket, Msg, CMLOG_Network)

	// check whether the thread is shutdown
	if (isThreadShutdown) 
		return false;

	// read data from peer
	const int32 RecvFlags = 0;
	const int32 BufferSize = 4096;
	CmString Data(BufferSize);
	CmString Info;
	int32 DataLength;
	while (SOCKET_ERROR == (DataLength = recv(ConnectionSocket, Data.getBuffer(), (int32)Data.getLength(), RecvFlags))){
		// check for buffer overrun
		int32 LastError = WSAGetLastError();
		if (WSAEMSGSIZE == LastError){
			// preserve received data
			Info += Data;
			continue;
		}
		// check for connection reset by peer
		if (WSAECONNRESET == LastError){
			// release socket
			closesocket(ConnectionSocket);
		}
		LOG6("ID=", ContactID, " error '", LastError, "' on socket ", ConnectionSocket, Msg2, CMLOG_Error)
		ConnectionSocket = INVALID_SOCKET;
		return false;
	}

	// isThreadShutdown or DataLength == 0 indicates that connection has to be closed
	if (isThreadShutdown || (0 == DataLength)){
		LOG4("ID=", ContactID, " finished connection on socket ", ConnectionSocket, Msg3, CMLOG_Network)
		return false;
	}

	// complete Info with received data
	Data.adjustLength(DataLength);
	Info += Data;

	// forward received Info to Gateway
	if (Info.getLength() > 0 && NULL != Gateway && false == Gateway->isShutdownGateway){
		LOG7("ID=", ContactID, " CmGatewayConnectionLAN::received data on socket ", ConnectionSocket, " Info='", Info, "'", Msg4, CMLOG_Network)

		Gateway->processInformation(Info);
	}

	return true;
}

bool CmGatewayConnectionLAN::isMatchingGatewayConnection(CmConnectionInfo& _GatewayInfo)
{
	// compare connection info
	if (RemoteAddr.sin_port != htons(_GatewayInfo.LAN.DstPort) ||
		RemoteAddr.sin_addr.S_un.S_addr != htonl(_GatewayInfo.LAN.DstHost))	{
		return false;
	}

	return true;
}

bool CmGatewayConnectionLAN::sendInfoToGateway(CmString& _Info)
{
		// send data to gateway
	if (SOCKET_ERROR == send(ConnectionSocket, _Info.getBuffer(), (int32)_Info.getLength(), 0)){
		LOG5("ID=", (NULL != Gateway ? Gateway->getContactID() : 0), " sending data on socket ", ConnectionSocket, " failed", Msg1, CMLOG_Error)
		return false;
	}

	return true;
}

