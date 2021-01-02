//////////////////////////////////////////////////////////////////////////////
//
// CmBots.cpp: Implementation of the CmBots classes
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

#define CmBots_Config
#include "Bots/CmBots.h"

//----------------------------------------------------------------------------
// PROVIDER_CmBots                                                PROVIDER
//----------------------------------------------------------------------------
//
PROVIDER_CmBots::PROVIDER_CmBots()
// Initialize PROVIDER UURI (=functionality) for a 'CmBots' root UURI
: CmPlugNode(UURI_PROVIDER_CmBots, UURI_CmBots)
{
	// initialize workspace parameters
	Pro = NULL;
	Dyn = NULL;
	Ctr = NULL;
	Msg = NULL;
	Pln = NULL;
	Rdn = NULL;

	// initialize CmValueINI arrays
	CmValueINI::setDefaultInfoFTL(pro_.UURI);
	CmValueINI::setDefaultInfoFTL(dyn_.UURI);
	CmValueINI::setDefaultInfoFTL(ctr_.UURI);
	CmValueINI::setDefaultInfoFTL(msg_.UURI);
	CmValueINI::setDefaultInfoFTL(pln_.UURI);
	CmValueINI::setDefaultInfoFTL(rdn_.UURI);
}

PROVIDER_CmBots::~PROVIDER_CmBots()
{
	// delete config data
	NULL != Pro ? delete Pro : 0;
	NULL != Dyn ? delete Dyn : 0;
	NULL != Ctr ? delete Ctr : 0;
	NULL != Msg ? delete Msg : 0;
	NULL != Pln ? delete Pln : 0;
	NULL != Rdn ? delete Rdn : 0;

}

//------Bot-test--------------------------------------------------PROVIDER----
bool PROVIDER_CmBots::testBot()
{
	// ToDo: 

	return true;
}


//------Remote-service-access-------------------------------------PROVIDER----
bool PROVIDER_CmBots::processInformation(CmString& _Information)
{
	// update local profile
	CmValueFTL InfoFTL;
	InfoFTL.processStringFTL(_Information);

	// ToDo: process information

	return true;
}

//------Background-data-processing--------------------------------PROVIDER----
bool PROVIDER_CmBots::runParallel()
{
	enterSerialize();

	// ToDo...

	leaveSerialize();

	return true;
}

//------Bot-functionality-----------------------------------------PROVIDER----

// ToDo...



//------CONFIGURATION-MANAGEMENT-functions------------------------PROVIDER----
CmBotsProfile& PROVIDER_CmBots::pro()
{
	return newConfig<CmBotsProfile>(&Pro, pro_);
}
bool PROVIDER_CmBots::updateProfile(CmStringFTL& _ProFTL)
{
	return pro().UURI.updateInfoFTL(_ProFTL, getReturn());
}
bool PROVIDER_CmBots::writeProfile(CmString _ConfigPath)
{
	CmStringFTL ProFTL;
	NULL != pro().UURI.getStringINI() ? ProFTL.processStringFTL(*pro().UURI.getStringINI()) : 0;
	return pro().UURI.writeInfoFTL(_ConfigPath, ProFTL, getReturn());
}
bool PROVIDER_CmBots::readProfile(CmString _ConfigPath)
{
	if (false == pro().UURI.readInfoFTL(_ConfigPath, getReturn())) return false;
	return true;
}
bool PROVIDER_CmBots::setDefaultProfile()
{
	return CmValueINI::setDefaultInfoFTL(pro().UURI, getReturn());
}

// Dynamic
CmBotsDynamic& PROVIDER_CmBots::dyn()
{
	return newConfig<CmBotsDynamic>(&Dyn, dyn_);
}
bool PROVIDER_CmBots::updateDynamic(CmStringFTL& _DynFTL)
{
	return dyn().UURI.updateInfoFTL(_DynFTL, getReturn());
}
bool PROVIDER_CmBots::writeDynamic(CmString _ConfigPath)
{
	CmStringFTL DynFTL;
	NULL != dyn().UURI.getStringINI() ? DynFTL.processStringFTL(*dyn().UURI.getStringINI()) : 0;
	return dyn().UURI.writeInfoFTL(_ConfigPath, DynFTL, getReturn());
}

// Control
CmBotsControl& PROVIDER_CmBots::ctr()
{
	return newConfig<CmBotsControl>(&Ctr, ctr_);
}
bool PROVIDER_CmBots::updateControl(CmStringFTL& _CtrFTL)
{
	return ctr().UURI.updateInfoFTL(_CtrFTL, getReturn());
}
bool PROVIDER_CmBots::writeControl(CmString _ConfigPath)
{
	CmStringFTL CtrFTL;
	NULL != ctr().UURI.getStringINI() ? CtrFTL.processStringFTL(*ctr().UURI.getStringINI()) : 0;
	return ctr().UURI.writeInfoFTL(_ConfigPath, CtrFTL, getReturn());
}

// Message
CmBotsMessage& PROVIDER_CmBots::msg()
{
	return newConfig<CmBotsMessage>(&Msg, msg_);
}
bool PROVIDER_CmBots::updateMessage(CmStringFTL& _MsgFTL)
{
	return msg().UURI.updateInfoFTL(_MsgFTL, getReturn());
}
bool PROVIDER_CmBots::writeMessage(CmString _ConfigPath)
{
	CmStringFTL MsgFTL;
	NULL != msg().UURI.getStringINI() ? MsgFTL.processStringFTL(*msg().UURI.getStringINI()) : 0;
	return msg().UURI.writeInfoFTL(_ConfigPath, MsgFTL, getReturn());
}

// Polling
CmBotsPolling& PROVIDER_CmBots::pln()
{
	return newConfig<CmBotsPolling>(&Pln, pln_);
}
bool PROVIDER_CmBots::updatePolling(CmStringFTL& _PlnFTL)
{
	return pln().UURI.updateInfoFTL(_PlnFTL, getReturn());
}
bool PROVIDER_CmBots::writePolling(CmString _ConfigPath)
{
	CmStringFTL PlnFTL;
	NULL != pln().UURI.getStringINI() ? PlnFTL.processStringFTL(*pln().UURI.getStringINI()) : 0;
	return pln().UURI.writeInfoFTL(_ConfigPath, PlnFTL, getReturn());
}

// Reading
CmBotsReading& PROVIDER_CmBots::rdn()
{
	return newConfig<CmBotsReading>(&Rdn, rdn_);
}
bool PROVIDER_CmBots::updateReading(CmStringFTL& _RdnFTL)
{
	return rdn().UURI.updateInfoFTL(_RdnFTL, getReturn());
}
bool PROVIDER_CmBots::writeReading(CmString _ConfigPath)
{
	CmStringFTL RdnFTL;
	NULL != rdn().UURI.getStringINI() ? RdnFTL.processStringFTL(*rdn().UURI.getStringINI()) : 0;
	return rdn().UURI.writeInfoFTL(_ConfigPath, RdnFTL, getReturn());
}

// Diagnostics
bool PROVIDER_CmBots::clearLogLevel()
{
	// clear log/message/context
	dyn().LogLevel = CMLOG_None;
	CmString(dyn().Message) = "";
	CmString(dyn().Context) = "";

	return true;
}
CmValueINI& PROVIDER_CmBots::getReturn()
{
	// return first item of a LogLevel/Message/Context config sequence
	return dyn().LogLevel;
}
// UURI
CmUURI& PROVIDER_CmBots::getUURI()
{
	// Actual PROVIDER implementation of the 'getUURI()' SERVICE function
	return ServiceUURI;
}


//----------------------------------------------------------------------------
// BOT_CmBots
//----------------------------------------------------------------------------
//
BOT_CmBots::BOT_CmBots(const int8 *_Init)
	:CmValueINI(_Init)
{
	// assign bot name
	CmString BotName = getText();
	setBotName(BotName);

}
BOT_CmBots::~BOT_CmBots()
{

}

//----------------------------------------------------------------------------
// SERVICE_CmBots
//----------------------------------------------------------------------------
//
SERVICE_CmBots::SERVICE_CmBots()
// Initialize SERVICE UURI (=interface) for a 'CmBots' root UURI
: CmPlugNode(UURI_SERVICE_CmBots, UURI_CmBots)
{
	// Initialize
	LocalProvider = NULL;
}

SERVICE_CmBots::~SERVICE_CmBots()
{
	// Delete a local SERVICE implementation (PROVIDER) if it exists
	if (NULL != LocalProvider)
		delete LocalProvider;
}

//------Bot-test--------------------------------------------------------------
bool SERVICE_CmBots::testBot()
{
	return Provider().testBot();
}

//------Remote-service-access-------------------------------------------------
bool SERVICE_CmBots::processInformation(CmString& _Information)
{
	return Provider().processInformation(_Information);
}

//------Bot-functionality-----------------------------------------------------

// ToDo...





//------CONFIGURATION-MANAGEMENT-functions------------------------------------

// profile
CmBotsProfile& SERVICE_CmBots::pro()
{
	return Provider().pro();
}
bool SERVICE_CmBots::updateProfile(CmStringFTL& _ProFTL)
{
	return Provider().updateProfile(_ProFTL);
}
bool SERVICE_CmBots::writeProfile(CmString _ConfigPath)
{
	return Provider().writeProfile(_ConfigPath);
}
bool SERVICE_CmBots::readProfile(CmString _ConfigPath)
{
	return Provider().readProfile(_ConfigPath);
}
bool SERVICE_CmBots::setDefaultProfile()
{
	return Provider().setDefaultProfile();
}
// dynamic
CmBotsDynamic& SERVICE_CmBots::dyn()
{
	return Provider().dyn();
}
bool SERVICE_CmBots::updateDynamic(CmStringFTL& _dyn)
{
	return Provider().updateDynamic(_dyn);
}
bool SERVICE_CmBots::writeDynamic(CmString _ConfigPath)
{
	return Provider().writeDynamic(_ConfigPath);
}
// control
CmBotsControl& SERVICE_CmBots::ctr()
{
	return Provider().ctr();
}
bool SERVICE_CmBots::updateControl(CmStringFTL& _ctr)
{
	return Provider().updateControl(_ctr);
}
bool SERVICE_CmBots::writeControl(CmString _ConfigPath)
{
	return Provider().writeControl(_ConfigPath);
}
// message
CmBotsMessage& SERVICE_CmBots::msg()
{
	return Provider().msg();
}
bool SERVICE_CmBots::updateMessage(CmStringFTL& _msg)
{
	return Provider().updateMessage(_msg);
}
bool SERVICE_CmBots::writeMessage(CmString _ConfigPath)
{
	return Provider().writeMessage(_ConfigPath);
}
// polling
CmBotsPolling& SERVICE_CmBots::pln()
{
	return Provider().pln();
}
bool SERVICE_CmBots::updatePolling(CmStringFTL& _pln)
{
	return Provider().updatePolling(_pln);
}
bool SERVICE_CmBots::writePolling(CmString _ConfigPath)
{
	return Provider().writePolling(_ConfigPath);
}
// reading
CmBotsReading& SERVICE_CmBots::rdn()
{
	return Provider().rdn();
}
bool SERVICE_CmBots::updateReading(CmStringFTL& _rdn)
{
	return Provider().updateReading(_rdn);
}
bool SERVICE_CmBots::writeReading(CmString _ConfigPath)
{
	return Provider().writeReading(_ConfigPath);
}
// synchronization / diagnostics 
bool SERVICE_CmBots::enterSerialize(int32 _LockID)
{
	// lock provider and service
	if (false == Provider().enterSerialize(_LockID)) return false;
	return CmParallelFTL::enterSerialize(_LockID);
}
bool SERVICE_CmBots::leaveSerialize()
{
	// unlock provider and service
	if (false == Provider().leaveSerialize()) return false;
	return CmParallelFTL::leaveSerialize();
}
bool SERVICE_CmBots::clearLogLevel()
{
	return Provider().clearLogLevel();
}
CmString& SERVICE_CmBots::setBotName(CmString _BotName)
{
	// store service UURI and bot name in the SERVICE plugnode
	CmPlugNode::ServiceUURI = Provider().getUURI().getText();
	CmPlugNode::setBotName(_BotName);

	return CmPlugNode::getBotUURI();
}
CmString& SERVICE_CmBots::getBotUURI()
{
	return CmPlugNode::getBotUURI();
}
CmUURI& SERVICE_CmBots::getUURI()
{
	return Provider().getUURI();
}

//------Service-access-by-PROVIDER--------------------------------------------

PROVIDER_CmBots& SERVICE_CmBots::Provider()
{
	// Instantiate a PROVIDER class if it does not exist yet
	if (NULL == ServiceProvider)
	{
		LocalProvider = new PROVIDER_CmBots();
		ServiceProvider = LocalProvider;
	}

	// Throw exception in case of memory problems
	if (NULL == ServiceProvider)
		throw(0);

	return *(PROVIDER_CmBots *)ServiceProvider;
}

