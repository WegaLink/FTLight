//////////////////////////////////////////////////////////////////////////////
//
// CmReturn.h - Declaration of return codes in Cm namespace
//
//////////////////////////////////////////////////////////////////////////////
//
// author: 	   Eckhard Kantz
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

#ifndef CmReturnH
#define CmReturnH

namespace Cosmos
{

/** CmReturn
 *  Any UURI can be used as a return value. The NULL value denotes success.
 */
class CmUURI;



/** CmReturnUURI
 *  Return values in Cm namespace are defined in this central location for all
 *  program modules that comprise the Cm software.
 *
 *  All return values are organized in groups Cm_00 .. Cm_NN. A group will
 *  represent all return values of a program module. Actual return values will
 *  be enumerated 0,1,..,N inside each group.
 */

#define Cm_00       0000
#define Cm_01       0100
#define Cm_02       0200
#define Cm_03       0300
#define Cm_04       0400
#define Cm_05       0500
#define Cm_06       0600
#define Cm_07       0700
#define Cm_08       0800
#define Cm_09       0900
#define Cm_10       1000

enum CmReturnUURI
{
  // CmTypes (general return codes)
  Cm_OK                                             = Cm_00 + 0,
  Cm_Error                                          = Cm_00 + 1,

  // CmPulseData
  CmPulseData_FileCouldNotBeWritten                 = Cm_01 + 0,

};

typedef CmReturnUURI CmReturn;

}  
#endif // ifndef CmReturnH
