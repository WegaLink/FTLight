//////////////////////////////////////////////////////////////////////////////
//
// CmStream.cpp: Implementation of the CmStream class
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

#include "Services/PlugNode/CmStream.h"

using namespace Cosmos;


//------------------------------------------------------------------------------
// CmStream class
//------------------------------------------------------------------------------
CmStream::CmStream(int64 _Window)
{
  // Create a stream buffer (data window)
  WindowSize = _Window;
  DataWindow = new uint8[(uint32)WindowSize];

  // Initialize bookkeeping data
  WritePosition = 0;          // next write position in the stream
  ReadPosition = 0;           // next read position in the stream
  WrapPosition = WindowSize;  // stream position on which the data window wraps from end to begin
  NewLinePosition = 0;        // next 'new line' (0D0A) ahead of read position
  isNextNewLine = false;      // whether a next 'new line' (0D0A) has already been determined

  // Initialize gap monitoring
  MaxGapCount = CMSTREAM_MAX_GAP_COUNT;
  GapStore = new struct DataGap[MaxGapCount];
  GapIdle = GapStore;
  GapMonitor = NULL;
  for( int i = 0; i < MaxGapCount - 1; i++ )
  {
    GapStore[i].NextGap = NULL;
    GapStore[i].GapStart = 0;
    GapStore[i].GapEnd = 0;
    if( i > 0 )
      GapStore[i-1].NextGap = &GapStore[i];
  }

  // Initialize stream callback
	StreamReceiver = this;
}

CmStream::~CmStream()
{
  // Delete stream buffer
  if( NULL!=DataWindow )
    delete[] DataWindow;

  // Remove gap monitoring
  if( NULL!=GapStore )
    delete[] GapStore;  
}

bool CmStream::processMessage(CmStream& /*_Message*/)
{
	// this function receives messages. However, here it is empty because
	// the actual work is being performed in derived classes.
	
	return true;
}

bool CmStream::writeData(uint8 *_Data, int64 _Size, int64 /*_Position*/)
{
	// Check if the data window fits into the range of 32-bit integer values
	if( WindowSize > MAX_INT32 )
	{
	// ToDo: handle huge data window > 2 GByte

	return false;
	}

	// Determine free buffer size in the data window of the stream buffer
	int64 FreeBuffer = WindowSize - (WritePosition - ReadPosition);

	if( FreeBuffer < _Size )
	{
	// ToDo: Split data into chunks that fit into available data window

	return false;
	}

	// Transfer data according to one of three possible situations. It is common
	// to all cases that the amount of data completely fits into stream's free
	// buffer space. However, three situation have to be taken into account
	// dependent on the sequence of read and write positions in the buffer:
	//
	// (1) read..write..wrap (size < wrap - write):
	//       write complete data to write position
	//
	// (2) read..write..wrap (size >= wrap - write):
	//       write first data chunk from write to wrap position and put remaining
	//       data to the beginning of the data window
	//
	// (3) write..read..wrap:
	//       write complete data to write position
	//
	// Check data size against wrap position
	if( WritePosition + _Size <= WrapPosition  )
	{
	// Determine write position in the data window
	int WindowWritePosition = (int)(WindowSize - (WrapPosition - WritePosition));

	// Just copy data into stream buffer
	memcpy(DataWindow + WindowWritePosition,_Data,(size_t)_Size);
	}
	else
	// Check write position against wrap position
	if( WritePosition < WrapPosition )
	{
	// Determine write position in the data window
	int WindowWritePosition = (int)(WindowSize - (WrapPosition - WritePosition));

	// Copy a first data chunk to the end of stream buffer
	int FreeBufferToWrapPosition = (int)(WrapPosition - WritePosition);
	memcpy(DataWindow + WindowWritePosition,_Data,FreeBufferToWrapPosition);

	// Copy remaining data chunk to the begin of stream buffer
	int RemainingData = (int)(_Size - FreeBufferToWrapPosition);
	memcpy(DataWindow,_Data + FreeBufferToWrapPosition,RemainingData);
	}
	else
	// Write position is behind of read position in the data window (wrap)
	{
	// Determine write position in the data window
	int WindowWritePosition = (int)(WritePosition - WrapPosition);

	// Just copy data into stream buffer
	memcpy(DataWindow + WindowWritePosition,_Data,(size_t)_Size);
	}

	// Step write position forward
	WritePosition += _Size;

	// Run stream callback whenever a complete line has become available
	if (NULL != StreamReceiver && isReadLine())
		StreamReceiver->processMessage(*this);

	return true;
}

bool CmStream::writeLine(const CmString& _Line)
{
  // Write next line into buffer
	if (false == writeData((uint8 *)_Line.getBuffer(), _Line.getLength())) return false;

  // Delimiter
	if (false == writeData((uint8 *)CMSTREAM_LINE_DELIMITER, CMSTREAM_LINE_DELIMITER_LENGTH)) return false;

  return true;
}

__int64 CmStream::readLine(CmString& _Line)
{
  // Check data availability
  if( isReadLine() )
  {
    // Determine data length
    uint32 ReadLength = (uint32)(NewLinePosition - ReadPosition);
    _Line.setLength(ReadLength);

    // Copy data into string
    if( ReadPosition + ReadLength <= WrapPosition )
    {
      // Determine start position in DataWindow and copy data in one go
      int64 StartInDataWindow = WindowSize - (WrapPosition - ReadPosition);
      memcpy(_Line.getBuffer(),DataWindow + StartInDataWindow,(size_t)ReadLength);
    }
    else
    {
      // Copy a first data chunk until wrap position
      int64 StartInDataWindow = WindowSize - (WrapPosition - ReadPosition);
      int64 LengthWrap = WrapPosition - ReadPosition;
      memcpy(_Line.getBuffer(),DataWindow + StartInDataWindow,(size_t)LengthWrap);

      // Copy a second data chunk after wrap position
      memcpy(_Line.getBuffer() + LengthWrap,DataWindow,(size_t)(ReadLength - LengthWrap));
    }

    // Step forward in stream for reading data
    NewLinePosition += 2;
    ReadPosition = NewLinePosition;
    isNextNewLine = false;

    // Switch wrap position forward by DataWindow size if a wrap took place
    if( ReadPosition >= WrapPosition )
      WrapPosition += WindowSize;
  }
  else
    _Line = "Hello, no data available";

  return ReadPosition;
}

bool CmStream::isReadLine()
{
  // Check if a next 'new line' (0D0A) ahead of read position has already been determined
  if( isNextNewLine )
    return true;

  // Check if the data window fits into the range of 32-bit integer values
  if( WindowSize > MAX_INT32 )
  {
    // ToDo: handle huge data window > 2 GByte

    return false;
  }

  /** Start/continue searching a next new line in available data
   *  The search will start from last search position. It will continue to the
   *  last position before next write position. Thus it will involve all data
   *  that is currently available in the DataWindow.
   *  The search may possibly run in two parts, one from last search position
   *  to wrap position and a second run from begin of DataWindow to the last
   *  position before next write position.
   *
   *    Search1:   NewLinePosition...(WrapPosition-1)
   *    Search2:   BeginOfDataWindow...(WritePosition-1)
   */
  int64 StartSearch1 = 0;
  int64 StartSearch2 = 0;
  int64 EndSearch1 = 0;
  int64 EndSearch2 = 0;
  int64 NewLineWindowPosition = 0;
  bool isSearchWrap = false;
  if( NewLinePosition >= WritePosition )
  {
    // There is no data available for starting/continuing with a 'new line' (0D0A) search
    return false;
  }
  else
  if( WritePosition <= WrapPosition )
  {
    // Search for 'new line' (0D0A) from last search position to write position
    StartSearch1 = WindowSize - (WrapPosition - NewLinePosition);
    EndSearch1 = WindowSize - (WrapPosition - WritePosition) - 1;
  }
  else
  {
    // Wrap took place: the write position is already beyond WrapPosition
    if( NewLinePosition >= WrapPosition )
    {
      // Search for 'new line' (0D0A) from last search position to write position
      StartSearch1 = NewLinePosition - WrapPosition;
      EndSearch1 = WritePosition - WrapPosition - 1;
      NewLineWindowPosition = WindowSize;
    }
    else
    {
      // Run a first search from last search position to wrap position and
      // continue with a second search from begin of DataWindow to write
      // position in case the first search was not successful
      StartSearch1 = WindowSize - (WrapPosition - NewLinePosition);
      EndSearch1 = WindowSize - 1;
      StartSearch2 = 0;
      EndSearch2 = WritePosition - WrapPosition - 1;
      isSearchWrap = true;
    }
  }

  // Run search for 'new line' (0D0A)
  void *NewLine;
  NewLine = memchr(DataWindow + StartSearch1,0x0D,(size_t)(EndSearch1 - StartSearch1 + 1));
  if( NULL==NewLine && isSearchWrap )
  {
    // Continue searching from start of DataWindow (wrap search)
    NewLineWindowPosition = WindowSize;
    NewLine = memchr(DataWindow + StartSearch2,0x0D,(size_t)(EndSearch2 - StartSearch2 + 1));
  }

  // Adjust bookkeeping data if no 'new line' (0D0A) has been found
  if( NULL==NewLine )
  {
    NewLinePosition = WritePosition;
    return false;
  }

  // Complete search for 'new line' (0D0A)
  uint8 *LastWindowPosition = DataWindow + (int)(WindowSize - 1);
  if( NewLine != LastWindowPosition )
    isNextNewLine = *((uint8 *)NewLine+1) == 0x0A; // evaluate next position
  else
    isNextNewLine = *DataWindow == 0x0A;  // wrap to begin of DataWindow

  // Check if a 'new line' (0D0A) has been found
  if( isNextNewLine )
  {
    // Determine found 'new line' (0D0A) position in DataWindow
    NewLineWindowPosition += (uint8 *)NewLine - DataWindow;

    // Mark found 'new line' (0D0A) position in the stream
    NewLinePosition = WrapPosition - WindowSize + NewLineWindowPosition;
  }

  return true;
}

bool CmStream::registerStreamReceiver(CmStream* _StreamReceiver)
{
  // Check parameters
	if (NULL == _StreamReceiver) return false;

  // Check that an external stream receiver has not been registered already before
	if (NULL!=StreamReceiver && StreamReceiver!=this) return false;

  // Accept registration
	StreamReceiver = _StreamReceiver;

  return true;
}

bool CmStream::unregisterStreamReceiver(CmStream* _StreamReceiver)
{
  // Check parameters
	if (NULL == _StreamReceiver) return false;

  // Check that registered callback matches arguments
	if (_StreamReceiver != StreamReceiver) return false;

  // Clear registration
	StreamReceiver = NULL;

  return true;
}


