//////////////////////////////////////////////////////////////////////////////
//
// CmStream.h: Declaration of CmStream class
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

#ifndef CmStreamH
#define CmStreamH

#include "FTLight/CmStringFTL.h"

namespace Cosmos
{
/** CmStream.
 *  The CmStream class implements a stream concept based on text lines. It
 *  encompasses a buffer where data can be written to and received from.
 *  Further, bookkeeping information allows to determine a stream position upto
 *  which data is available to the user of the stream.
 *
 *  Additionally, a bookkeeping for gaps is implemented. That information is
 *  used for example when receiving data over a network by packets which can
 *  get lost. At any time, an analysis can be done which delivers information
 *  about a stream position upto which complete data has arrived. Further, a
 *  packet resend can be initiated whenever the receiving side decides that
 *  this would be appropriate e.g. when a timeout for missing data has elapsed.
 *
 *  NOTE: The CmStream class has been designed for transferring 'FTLight' coded
 *        data. The following link points to a 'FTLight' documentation:
 *          http://wegalink.eu/development/FTLight/FTLight_DE.pdf
 *
 *  Basically, the 'FTLight' specification demands for a document/stream content
 *  that is plain text. A 'carriage return + line feed' (0D0A) char sequence
 *  delimits particular text lines from each other. Structured data as well as
 *  binary data are coded in 'FTLight' based on plain text lines as it has been
 *  described in above mentioned 'FTLight' documentation.
 *
 *  A CmStream class takes plain text line by line and it adds a delimiter (0D0A)
 *  automatically. Therefore any text format using same delimiters (0D0A) can be
 *  transferred as well by this class. Each particular line (without 0D0A) should
 *  be forwarded in this case using e.g. the writeLine() function.
 */

class CmStream : public CmParallelFTL
{
#define CMSTREAM_DEFAULT_DATA_WINDOW     (128*1024)
#define CMSTREAM_LINE_DELIMITER           "\x0D\x0A"
#define CMSTREAM_LINE_DELIMITER_LENGTH    2

public:
	CmStream(int64 Window = CMSTREAM_DEFAULT_DATA_WINDOW);
protected:
	~CmStream();

public:
	/** processMessage.
	 *  A message will be processed on arrival. This funtion will be overridden by
	 *  derived classes which are supposed to do the actual work. 
	 *
	 * NOTE: Message must not be 'const' in order to be processed by readLine().
	 */
	virtual bool processMessage(CmStream& Message);

public:
  /** writeData.
   *  Data in the stream's buffer will be extended by new data. A stream position
   *  can be specified where the data will be written to. In case of any gaps
   *  that would result from data arriving ahead of current write position,
   *  those gaps will be registered and monitored.
   *
   *  @param Data a pointer to data that will be transferred to the stream buffer
   *  @param Size the amount of data to be added to the stream buffer
   *  @param Position the start address for writing to the stream (0 = current)
   *  @return whether writing succeeded or not
   */
  bool writeData(uint8 *Data,int64 Size,int64 Position =0);

  /** writeLine.
   *  A string will be appended to the end of the stream and a 'new line' (0D0A)
   *  will automatically be added.
   *
   *  @param Line a string that will be transferred to the stream buffer
   *  @return whether writing the line succeeded or not
   */
	bool writeLine(const CmString& Line);

  /** readLine.
   *  A string will be returned spanning the buffer positions from current read
   *  position to the next 'new line' (0D0A). The 'new line' (0D0A) will not be
   *  returned.
   *  If there is already a 'new line' (0D0A) present in the stream buffer after
   *  current read position then the function will return with respective string.
   *  Otherwise this will be a blocking call until a 'new line' (0D0A) was
   *  received from peer.
   *
   *  HINT: The 'isReadLine()' function can be used to determine whether a
   *        'new line' delimiter (0D0A) is present ahead of current read
   *        position for receiving a next valid line from the string.
   *
   *  @param Line the string that was extracted from stream buffer
   *  @return the resulting next read position in the stream
   */
	int64 readLine(CmString& Line);

  /** isReadLine.
   *  The stream buffer will be checked whether there is a next line available
   *  for reading starting from current read position to a 'new line' (0D0A).
   */
  bool isReadLine();

	/**  registerStreamReceiver.
   *   A stream receiver will be registered which makes a callback to a user
   *   defined function whenever a next complete line has arrived from a peer
   *   CmStream instance.
   */
	bool registerStreamReceiver(CmStream* StreamReceiver);

  /**  unregisterStreamReceiver.
   *   A stream receiver function will be unregistered. The value of the 
   *   StreamReceiver has to match the value when registering the
   *   callback. Otherwise the function will fail.
   */
	bool unregisterStreamReceiver(CmStream* StreamReceiver);

private:
  // Stream buffer (a data window)
  uint8 *DataWindow;
  int64 WindowSize;

  // Data bookkeeping
  int64 WritePosition;
  int64 ReadPosition;
  int64 WrapPosition;
  int64 NewLinePosition;
  bool isNextNewLine;

  // Gap monitoring
#define CMSTREAM_MAX_GAP_COUNT 100
  int MaxGapCount;
  struct DataGap
  {
    DataGap *NextGap;
    int64 GapStart;
    int64 GapEnd;
  };
  DataGap *GapMonitor;
  DataGap *GapIdle;
  DataGap *GapStore;

private:
	CmStream *StreamReceiver;
};


} // namespace Cosmos

using namespace Cosmos;

#endif // !defined(CmStreamH)
