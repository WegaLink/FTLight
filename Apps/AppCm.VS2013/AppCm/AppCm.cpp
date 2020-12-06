//////////////////////////////////////////////////////////////////////////////
//
// AppCm.cpp: Implementation of the AppCm application (App)
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

#define AppCm_Config
#include "AppCm.h"

/*------PROVIDER_AppCm------------ToDo----------------------------------------
*
* - restore a damaged config file to a default state
*
*/

//----------------------------------------------------------------------------
// PROVIDER_AppCm                                              PROVIDER
//----------------------------------------------------------------------------
//
PROVIDER_AppCm::PROVIDER_AppCm()
// Initialize PROVIDER UURI (=functionality) for a 'AppCm' root UURI
: CmPlugNode(UURI_PROVIDER_AppCm, UURI_AppCm)
{
	// initialize CmValueINI arrays
	CmValueINI::setDefaultInfoFTL(pro.UURI, getReturn());
	CmValueINI::setDefaultInfoFTL(dyn.UURI, getReturn());
	CmValueINI::setDefaultInfoFTL(ctr.UURI, getReturn());
	CmValueINI::setDefaultInfoFTL(msg.UURI, getReturn());
	CmValueINI::setDefaultInfoFTL(pln.UURI, getReturn());
	CmValueINI::setDefaultInfoFTL(rdn.UURI, getReturn());

	// Initialize workspace
	FuncUpdateGUI = (UpdateGUI)NULL;
	isShutDown = false;
	isDraw = true;


}
PROVIDER_AppCm::~PROVIDER_AppCm()
{

}

//------GUI-content-----------------------------------------------PROVIDER----
bool PROVIDER_AppCm::processMessageGUI(CmString _Key, CmString _Text, double _Value)
{
	// distribute GUI message
	if (isShutDown){
		return true;
	}
	else	if (_Key == "Update"){
		if (isDraw){
			isDraw = false;
			drawTouch();
			drawLocal();
			drawRemote();
		}
	}
	else if (_Key == "Draw"){
		drawTouch();
		drawLocal();
		drawRemote();
	}
	else if (_Key == "Save"){
		clearLogLevel();
		if (false == writeProfile(_Text)) return false;
	}
	else	if (_Key == "Parallel"){
		if (false == setParallel(_Text)) return false;
	}
	else if (_Key == "Close"){
		isShutDown = true;
		stopParallel();
		clearLogLevel();
		if (false == writeProfile(_Text)) return false;
#ifdef _DEBUG
		if (false == writeDynamic(_Text)) return false;
		if (false == writeControl(_Text)) return false;
		if (false == writeMessage(_Text)) return false;
		if (false == writePolling(_Text)) return false;
		if (false == writeReading(_Text)) return false;
#endif
	}
	else if (_Key == "LocalDevice"){
		pro.LocalDevice = _Text; isDraw = true;
		pro.LocalChart.pro().SignalType = CmSignalType(int32(_Value));
		_Text == "OFF" ? pro.LocalChart.pro().SignalPhase = 0 : 0;
	}
	else if (_Key == "RemoteDevice"){
		pro.RemoteDevice = _Text; isDraw = true;
		pro.RemoteChart.pro().SignalType = CmSignalType(int32(_Value));
		_Text == "OFF" ? pro.RemoteChart.pro().SignalPhase = 0 : 0;
	}
	else if (_Key == "TouchMode"){
		pro.TouchMode = _Text; isDraw = true;
		_Text == "TEST" ? startParallel() : stopParallel();

	}

	// update GUI
	updateGUI(&pro, &dyn);

	return true;
}

//------GUI-helper-functions--------------------------------------PROVIDER----
bool PROVIDER_AppCm::touchData(CmString& /*_KeyCode*/)
{
	enterSerialize();


	leaveSerialize();

	return true;
}
bool PROVIDER_AppCm::drawLocal()
{
	enterSerialize();

	pro.LocalChart.drawLayout(pro.LocalDevice == "OFF" ? true : false);

	leaveSerialize();

	return true;
}
bool PROVIDER_AppCm::drawRemote()
{
	enterSerialize();

	pro.RemoteChart.drawLayout(pro.RemoteDevice == "OFF" ? true : false);

	leaveSerialize();

	return true;
}
bool PROVIDER_AppCm::drawTouch()
{
	int32 TextCount = 0;

	enterSerialize();

	// disable drawing
	pro.TouchChart.dyn().Init.setDrawingEnabled(false);

	// background
	pro.TouchChart.dyn().Init.setChartBackground(pro.TouchChart.pro().BackgroundColor, pro.TouchChart.pro().BackgroundAlpha);

	CmString PhaseLocal("    Local phase: ");
	CmString PhaseRemote("Remote phase: ");
	CmString Phase;
	CmString Bot1("Bot1: ");
	CmString Bot2("Bot2: ");
	// local
	Phase.double2String(pro.LocalChart.getSignalPhaseOffsetDeg(), 0);
	PhaseLocal += Phase;
	PhaseLocal += "°";
	// remote
	Phase.double2String(pro.RemoteChart.getSignalPhaseOffsetDeg(), 0);
	PhaseRemote += Phase;
	PhaseRemote += "°";
	// modules
	Bot1 += pro.Bot1.getBotUURI();

	// position/font
	float Top = pro.TouchChart.dyn().Height;
	const float Left = 20;
	const float Space = 25;
	const float FontSize = 15;

	pro.TouchChart.dyn().Text.setChartText(TextCount++, CmPoint2D(Left, Top -= Space), PhaseLocal, FontSize);
	pro.TouchChart.dyn().Text.setChartText(TextCount++, CmPoint2D(Left, Top -= Space), PhaseRemote, FontSize);
	Top -= Space;
	pro.TouchChart.dyn().Text.setChartText(TextCount++, CmPoint2D(Left, Top -= Space), Bot1, FontSize);

	// enable drawing
	pro.TouchChart.dyn().Init.setDrawingEnabled(true);

	leaveSerialize();

	return true;
}
bool PROVIDER_AppCm::setParallel(CmString& _Command)
{
	if (_Command == "Start"){
		startParallel();
	}
	else if (_Command == "Stop"){
		stopParallel();
	}

	return true;
}

//------Remote-service-access-------------------------------------PROVIDER----
bool PROVIDER_AppCm::processInformation(CmString& _Information)
{
	// update local profile
	CmValueFTL InfoFTL;
	InfoFTL.processStringFTL(_Information);

	// update state and GUI from local profile
	updateGUI();

	return true;
}

//------GUI-update------------------------------------------------PROVIDER----
bool PROVIDER_AppCm::setWindow(CmPoint _Position, CmSize _Size)
{
	// save window position and size in the profile
	pro.WindowTop = _Position.y();
	pro.WindowLeft = _Position.x();
	pro.WindowWidth = _Size.x();
	pro.WindowHeight = _Size.y();

	return true;
}
bool PROVIDER_AppCm::getWindow(CmPoint& _Position, CmSize& _Size, CmSize _MinSize, CmSize _ScreenSize)
{
	// retrieve window position and size from profile
	_Position.y() = pro.WindowTop;
	_Position.x() = pro.WindowLeft;
	_Size.x() = pro.WindowWidth;
	_Size.y() = pro.WindowHeight;

	// verify window size
	_Size.x() = _Size.x() < 0 ? _MinSize.x() : _Size.x() > _ScreenSize.x() ? _ScreenSize.x() : _Size.x();
	_Size.y() = _Size.y() < 0 ? _MinSize.y() : _Size.y() > _ScreenSize.y() ? _ScreenSize.y() : _Size.y();
	// verify window location
	const int32 MinVisible = 75;
	_Position.x() = _Position.x() < MinVisible - _Size.x() ? MinVisible - _Size.x() : _Position.x() > _ScreenSize.x() - MinVisible ? _ScreenSize.x() - MinVisible : _Position.x();
	_Position.y() = _Position.y() < 0 ? 0 : _Position.y() > _ScreenSize.y() - MinVisible ? _ScreenSize.y() - MinVisible : _Position.y();

	return true;
}
bool PROVIDER_AppCm::setCanvas(CmSize _SizeLocal, CmSize _SizeRemote, CmSize _SizeTouch)
{
	enterSerialize();

	// save canvas sizes
	pro.LocalChart.dyn().Width = _SizeLocal.x();
	pro.LocalChart.dyn().Height = _SizeLocal.y();
	pro.RemoteChart.dyn().Width = _SizeRemote.x();
	pro.RemoteChart.dyn().Height = _SizeRemote.y();
	pro.TouchChart.dyn().Width = _SizeTouch.x();
	pro.TouchChart.dyn().Height = _SizeTouch.y();

	leaveSerialize();

	return true;
}
bool PROVIDER_AppCm::getCanvas(CmSize& _SizeLocal, CmSize& _SizeRemote, CmSize& _SizeTouch)
{
	enterSerialize();

	// save canvas sizes
	_SizeLocal.x() = pro.LocalChart.dyn().Width;
	_SizeLocal.y() = pro.LocalChart.dyn().Height;
	_SizeRemote.x() = pro.RemoteChart.dyn().Width;
	_SizeRemote.y() = pro.RemoteChart.dyn().Height;
	_SizeTouch.x() = pro.TouchChart.dyn().Width;
	_SizeTouch.y() = pro.TouchChart.dyn().Height;

	leaveSerialize();

	return true;
}
bool PROVIDER_AppCm::updateGUI(AppCmProfile *_pro, AppCmDynamic *_dyn)
{
	bool Return = false;

	// Check if a callback function has been registered before
	if (NULL == FuncUpdateGUI) return Return;

	// check profile/dynamic and update GUI
	if (NULL != _pro && NULL != _dyn) {
		enterSerialize();
		Return = (FuncUpdateGUI)(*_pro, *_dyn);
		leaveSerialize();
		return Return;
	}

	// update GUI
	enterSerialize();
	Return = (FuncUpdateGUI)(pro, dyn);
	leaveSerialize();
	return Return;
}
// callback for GUI update
bool PROVIDER_AppCm::registerUpdateGUICallback(UpdateGUI _FuncUpdateGUI)
{
	return registerCallback(FuncUpdateGUI, _FuncUpdateGUI);
}
bool PROVIDER_AppCm::unregisterUpdateGUICallback(UpdateGUI _FuncUpdateGUI)
{
	return unregisterCallback(FuncUpdateGUI, _FuncUpdateGUI);
}

//------Background-data-processing--------------------------------PROVIDER----
bool PROVIDER_AppCm::runParallel()
{
	enterSerialize();

	// update test signals
	if (pro.TouchMode == "TEST"){
		pro.LocalDevice != "OFF" ? pro.LocalChart.drawSignal(CmSignalType(int32(pro.LocalChart.pro().SignalType))) : 0;
		pro.RemoteDevice != "OFF" ? pro.RemoteChart.drawSignal(CmSignalType(int32(pro.RemoteChart.pro().SignalType))) : 0;
		drawTouch();
	}
	else{
		pro.LocalChart.drawLayout(pro.LocalDevice == "OFF" ? true : false);
		pro.RemoteChart.drawLayout(pro.RemoteDevice == "OFF" ? true : false);
	}

	leaveSerialize();

	Sleep(40);	// ~25fps

	return true;
}

//------CONFIGURATION-MANAGEMENT-functions------------------------PROVIDER----
AppCmProfile& PROVIDER_AppCm::getProfile()
{
	return pro;
}
bool PROVIDER_AppCm::updateProfile(CmStringFTL& _ProFTL)
{
	return pro.UURI.updateInfoFTL(_ProFTL, getReturn());
}
bool PROVIDER_AppCm::writeProfile(CmString _ConfigPath)
{ 
	CmStringFTL ProFTL;
	NULL != pro.UURI.StringINI ? ProFTL.processStringFTL(*pro.UURI.StringINI) : 0;
	return pro.UURI.writeInfoFTL(_ConfigPath, ProFTL, getReturn());
}
bool PROVIDER_AppCm::readProfile(CmString _ConfigPath)
{
	if (false == pro.UURI.readInfoFTL(_ConfigPath, getReturn())) return false;
	// update LocalChart config
	pro.LocalChart.pro().Title = CmString(pro.LocalChart);
	pro.LocalChart.pro().BackgroundColor = CmString(pro.LocalChartBackgroundColor);
	pro.LocalChart.pro().TitleFontSize = pro.LocalChartFontSize.getNumAsDouble();
	pro.LocalChart.pro().TitleFontColor = CmString(pro.LocalChartFontColor);
	pro.LocalChart.pro().SignalLineColor = CmString(pro.LocalChartSignalColor);
	// update RemoteChart config
	pro.RemoteChart.pro().Title = CmString(pro.RemoteChart);
	pro.RemoteChart.pro().BackgroundColor = CmString(pro.RemoteChartBackgroundColor);
	pro.RemoteChart.pro().TitleFontSize = pro.RemoteChartFontSize.getNumAsDouble();
	pro.RemoteChart.pro().TitleFontColor = CmString(pro.RemoteChartFontColor);
	pro.RemoteChart.pro().SignalLineColor = CmString(pro.RemoteChartSignalColor);
	// update TouchChart config
	pro.TouchChart.pro().Title = CmString(pro.TouchChart);
	pro.TouchChart.pro().BackgroundColor = CmString(pro.TouchChartBackgroundColor);
	pro.TouchChart.pro().TitleFontSize = pro.TouchChartFontSize.getNumAsDouble();
	pro.TouchChart.pro().TitleFontColor = CmString(pro.TouchChartFontColor);
	pro.TouchChart.pro().SignalLineColor = CmString(pro.TouchChartSignalColor);
	// update Bots config
	pro.Bot1.setBotName(CmString(pro.Bot1));

	return true;
}
bool PROVIDER_AppCm::setDefaultProfile()
{
	return CmValueINI::setDefaultInfoFTL(pro.UURI, getReturn());
}
// Dynamic
AppCmDynamic& PROVIDER_AppCm::getDynamic()
{
	return dyn;
}
bool PROVIDER_AppCm::updateDynamic(CmStringFTL& _DynFTL)
{
	return dyn.UURI.updateInfoFTL(_DynFTL, getReturn());
}
bool PROVIDER_AppCm::writeDynamic(CmString _ConfigPath)
{
	CmStringFTL DynFTL;
	NULL != dyn.UURI.StringINI ? DynFTL.processStringFTL(*dyn.UURI.StringINI) : 0;
	return dyn.UURI.writeInfoFTL(_ConfigPath, DynFTL, getReturn());
}
// Control
AppCmControl& PROVIDER_AppCm::getControl()
{
	return ctr;
}
bool PROVIDER_AppCm::updateControl(CmStringFTL& _CtrFTL)
{
	return ctr.UURI.updateInfoFTL(_CtrFTL, getReturn());
}
bool PROVIDER_AppCm::writeControl(CmString _ConfigPath)
{
	CmStringFTL CtrFTL;
	NULL != ctr.UURI.StringINI ? CtrFTL.processStringFTL(*ctr.UURI.StringINI) : 0;
	return ctr.UURI.writeInfoFTL(_ConfigPath, CtrFTL, getReturn());
}
// Message
AppCmMessage& PROVIDER_AppCm::getMessage()
{
	return msg;
}
bool PROVIDER_AppCm::updateMessage(CmStringFTL& _MsgFTL)
{
	return msg.UURI.updateInfoFTL(_MsgFTL, getReturn());
}
bool PROVIDER_AppCm::writeMessage(CmString _ConfigPath)
{
	CmStringFTL MsgFTL;
	NULL != msg.UURI.StringINI ? MsgFTL.processStringFTL(*msg.UURI.StringINI) : 0;
	return msg.UURI.writeInfoFTL(_ConfigPath, MsgFTL, getReturn());
}
// Polling
AppCmPolling& PROVIDER_AppCm::getPolling()
{
	return pln;
}
bool PROVIDER_AppCm::updatePolling(CmStringFTL& _PlnFTL)
{
	return pln.UURI.updateInfoFTL(_PlnFTL, getReturn());
}
bool PROVIDER_AppCm::writePolling(CmString _ConfigPath)
{
	CmStringFTL PlnFTL;
	NULL != pln.UURI.StringINI ? PlnFTL.processStringFTL(*pln.UURI.StringINI) : 0;
	return pln.UURI.writeInfoFTL(_ConfigPath, PlnFTL, getReturn());
}
// Reading
AppCmReading& PROVIDER_AppCm::getReading()
{
	return rdn;
}
bool PROVIDER_AppCm::updateReading(CmStringFTL& _RdnFTL)
{
	return rdn.UURI.updateInfoFTL(_RdnFTL, getReturn());
}
bool PROVIDER_AppCm::writeReading(CmString _ConfigPath)
{
	CmStringFTL RdnFTL;
	NULL != rdn.UURI.StringINI ? RdnFTL.processStringFTL(*rdn.UURI.StringINI) : 0;
	return rdn.UURI.writeInfoFTL(_ConfigPath, RdnFTL, getReturn());
}

// Diagnostics
bool PROVIDER_AppCm::clearLogLevel()
{
	// clear log/message/context
	dyn.LogLevel = CMLOG_None;
	CmString(dyn.Message) = "";
	CmString(dyn.Context) = "";

	return true;
}
CmValueINI& PROVIDER_AppCm::getReturn()
{ 
	// return first item of a LogLevel/Message/Context config sequence
	return dyn.LogLevel; 
}
// UURI
CmUURI& PROVIDER_AppCm::getUURI()
{
	// Actual PROVIDER implementation of the 'getUURI()' SERVICE function
	return ServiceUURI;
}

//----------------------------------------------------------------------------
// SERVICE_AppCm
//----------------------------------------------------------------------------
//
SERVICE_AppCm::SERVICE_AppCm()
// Initialize SERVICE UURI (=interface) for a 'AppCm' root UURI
: CmPlugNode(UURI_SERVICE_AppCm, UURI_AppCm)
{
	// Initialize
	LocalProvider = NULL;

	// set log level
	setLogLevel(CMLOG_AppCm);
}
SERVICE_AppCm::~SERVICE_AppCm()
{
	// Delete a local SERVICE implementation (PROVIDER) if it exists
	if (NULL != LocalProvider)
		delete LocalProvider;
}

//------GUI-content-----------------------------------------------------------
bool SERVICE_AppCm::processMessageGUI(CmString _Key, CmString _Text, double _Value)
{
	return Provider().processMessageGUI(_Key, _Text, _Value);
}

//------Remote-service-access-------------------------------------------------
bool SERVICE_AppCm::processInformation(CmString& _Information)
{
	return Provider().processInformation(_Information);
}

//------GUI-update------------------------------------------------------------
bool SERVICE_AppCm::setWindow(CmPoint _Position, CmSize _Size)
{
	return Provider().setWindow(_Position, _Size);
}
bool SERVICE_AppCm::getWindow(CmPoint& _Position, CmSize& _Size, CmSize _MinSize, CmSize _ScreenSize)
{
	return Provider().getWindow(_Position, _Size, _MinSize, _ScreenSize);
}
bool SERVICE_AppCm::setCanvas(CmSize _SizeLocal, CmSize _SizeTouch, CmSize _SizeRemote)
{
	return Provider().setCanvas(_SizeLocal, _SizeTouch, _SizeRemote);
}
bool SERVICE_AppCm::getCanvas(CmSize& _SizeLocal, CmSize& _SizeTouch, CmSize& _SizeRemote)
{
	return Provider().setCanvas(_SizeLocal, _SizeTouch, _SizeRemote);
}
bool SERVICE_AppCm::updateGUI(AppCmProfile *_pro, AppCmDynamic *_dyn)
{
	return Provider().updateGUI(_pro, _dyn);
}

//------CONFIGURATION-MANAGEMENT-functions------------------------------------

// profile
AppCmProfile& SERVICE_AppCm::getProfile()
{
	return Provider().getProfile();
}
bool SERVICE_AppCm::updateProfile(CmStringFTL& _ProFTL)
{
	return Provider().updateProfile(_ProFTL);
}
bool SERVICE_AppCm::writeProfile(CmString _ConfigPath)
{
	return Provider().writeProfile(_ConfigPath);
}
bool SERVICE_AppCm::readProfile(CmString _ConfigPath)
{
	return Provider().readProfile(_ConfigPath);
}
bool SERVICE_AppCm::setDefaultProfile()
{
	return Provider().setDefaultProfile();
}
// dynamic
AppCmDynamic& SERVICE_AppCm::getDynamic()
{
	return Provider().getDynamic();
}
bool SERVICE_AppCm::updateDynamic(CmStringFTL& _dyn)
{
	return Provider().updateDynamic(_dyn);
}
bool SERVICE_AppCm::writeDynamic(CmString _ConfigPath)
{
	return Provider().writeDynamic(_ConfigPath);
}
// control
AppCmControl& SERVICE_AppCm::getControl()
{
	return Provider().getControl();
}
bool SERVICE_AppCm::updateControl(CmStringFTL& _ctr)
{
	return Provider().updateControl(_ctr);
}
bool SERVICE_AppCm::writeControl(CmString _ConfigPath)
{
	return Provider().writeControl(_ConfigPath);
}
// message
AppCmMessage& SERVICE_AppCm::getMessage()
{
	return Provider().getMessage();
}
bool SERVICE_AppCm::updateMessage(CmStringFTL& _msg)
{
	return Provider().updateMessage(_msg);
}
bool SERVICE_AppCm::writeMessage(CmString _ConfigPath)
{
	return Provider().writeMessage(_ConfigPath);
}
// polling
AppCmPolling& SERVICE_AppCm::getPolling()
{
	return Provider().getPolling();
}
bool SERVICE_AppCm::updatePolling(CmStringFTL& _pln)
{
	return Provider().updatePolling(_pln);
}
bool SERVICE_AppCm::writePolling(CmString _ConfigPath)
{
	return Provider().writePolling(_ConfigPath);
}
// reading
AppCmReading& SERVICE_AppCm::getReading()
{
	return Provider().getReading();
}
bool SERVICE_AppCm::updateReading(CmStringFTL& _rdn)
{
	return Provider().updateReading(_rdn);
}
bool SERVICE_AppCm::writeReading(CmString _ConfigPath)
{
	return Provider().writeReading(_ConfigPath);
}
// synchronization / diagnostics 
bool SERVICE_AppCm::enterSerialize(int32 _LockID)
{
	// lock provider and service
	if (false == Provider().enterSerialize(_LockID)) return false;
	return CmParallelFTL::enterSerialize(_LockID);
}
bool SERVICE_AppCm::leaveSerialize()
{
	// unlock provider and service
	if (false == Provider().leaveSerialize()) return false;
	return CmParallelFTL::leaveSerialize();
}
bool SERVICE_AppCm::clearLogLevel()
{
	return Provider().clearLogLevel();
}
// UURI
CmUURI& SERVICE_AppCm::getUURI()
{
	return Provider().getUURI();
}

//------Service-access-through-a-PROVIDER-------------------------------------

PROVIDER_AppCm& SERVICE_AppCm::Provider()
{
	// Lookup a PROVIDER class if it was not discovered before
	if (NULL == ServiceProvider)
	{
		LocalProvider = new PROVIDER_AppCm();
		ServiceProvider = LocalProvider;
	}

	// Throw exception in case of memory problems
	if (NULL == ServiceProvider)
		throw(0);

	return *(PROVIDER_AppCm *)ServiceProvider;
}
