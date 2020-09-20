//////////////////////////////////////////////////////////////////////////////
//
// CmTypes.cpp: Implementation of basic type classes
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

#include "Services/Basic/CmTypes.h"

// -> some basic functionality has gone to separate files
// - CmException
// - CmString
// - CmTime

//----------------------------------------------------------------------------
// class CmSmoothed
//----------------------------------------------------------------------------

CmSmoothed::CmSmoothed(double _StartValue)
{
	// set start value
	Value = _StartValue;

	// initialize default smoothing parameters
	Threshold = CMSMOOTHED_THRESHOLD_DEFAULT;
	Factor = CMSMOOTHED_FACTOR_DEFAULT;
}


CmSmoothed::~CmSmoothed()
{
}

double CmSmoothed::smooth(double _NewValue)
{
	// take threshold into account when smoothing new value
	if ((Threshold > 0.001) && ((_NewValue < Value*Threshold) || (_NewValue > Value / Threshold))) {
		Value = _NewValue;
	}else{
		Value = Value + Factor * (_NewValue - Value);
	}

	return Value;
}
double CmSmoothed::smooth(int32 _NewValue)
{
	return smooth((double)_NewValue);
}

