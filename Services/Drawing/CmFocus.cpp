//////////////////////////////////////////////////////////////////////////////
//
// CmFocus.cpp: Implementation of the CmFocus classes
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


#include "CmFocus.h"

using namespace Cosmos;

//------------------------------------------------------------------------------
// Class CmFocus - Constructor and destructor
//------------------------------------------------------------------------------
CmFocus::CmFocus()
{
  // Initialize focus list
  FocusList = NULL;

  // Initialize time settings
  StartTime = 0;
  EndTime = 0;
  IntervalLength = 0;
  IntervalCount = 0;

  // Initialize base class statistics
  BaseStatistics = NULL;
  PreviousValue = MIN_INT64;
}

CmFocus::~CmFocus()
{

}

//------------------------------------------------------------------------------
// Class CmFocus - Access functions to time settings
//------------------------------------------------------------------------------
void CmFocus::setStartTime(int64 StartTime)
{
  this->StartTime = StartTime;

  // Clear previous base statistics
  clearBaseStatistics();
}

void CmFocus::setEndTime(int64 EndTime)
{
  this->EndTime = EndTime;

  // Clear previous base statistics
  clearBaseStatistics();
}

int64 CmFocus::setIntervalLength(int64 IntervalLength)
{
  this->IntervalLength = IntervalLength;

  // Clear previous base statistics
  clearBaseStatistics();

  // Determine interval count
  if( IntervalLength > 0 && EndTime > StartTime )
    IntervalCount = ((EndTime - StartTime) / IntervalLength + 1);

  return IntervalCount;
}

int64 CmFocus::setIntervalCount(int64 IntervalCount)
{
  this->IntervalCount = IntervalCount;

  // Clear previous base statistics
  clearBaseStatistics();

  // Determine interval length
  if( IntervalCount > 0 && EndTime > StartTime )
    IntervalLength = (EndTime - StartTime) / IntervalCount;

  return IntervalLength;
}

int64 CmFocus::getStartTime()
{
  return StartTime;
}

int64 CmFocus::getEndTime()
{
  return EndTime;
}

int64 CmFocus::getIntervalLength()
{
  return IntervalLength;
}

int64 CmFocus::getIntervalCount()
{
  return IntervalCount;
}

//------------------------------------------------------------------------------
// Class CmFocus - Base statistiscs functions
//------------------------------------------------------------------------------
void CmFocus::clearBaseStatistics()
{
  // Destroy a previous base statistics array
  if( BaseStatistics )
    delete BaseStatistics;
  BaseStatistics = NULL;
}

bool CmFocus::verifyBaseStatistics()
{
  // Check if a base statistics array was already allocated
  if( !BaseStatistics )
  {
    // Check if valid start/end time was set
    if( !(StartTime < EndTime) )
      return false;

    // Check for valid interval length/count
    if( IntervalLength == 0 && IntervalCount == 0 )
      return false;

    // Determine interval length/count if needed
    if( IntervalLength == 0 && IntervalCount > 0 )
      IntervalLength = (EndTime - StartTime) / IntervalCount;

    if( IntervalCount == 0 && IntervalLength > 0 )
      IntervalCount = int32(((EndTime - StartTime) / IntervalLength + 1));

    // Check whether valid values for interval length/count are available
    if( IntervalLength == 0 || IntervalCount == 0 )
      return false;

    // Allocate a base statistics array
    BaseStatistics = new CmFocusBaseStatistics[size_t(IntervalCount)];

    // Check if a base statistics array was successfully allocated
    if( !BaseStatistics )
      return false;

    // Initialize base statistics array
    memset(BaseStatistics, 0, size_t(IntervalCount * sizeof(CmFocusBaseStatistics)));
    for( int32 i = 0; i < IntervalCount; i++ )
    {
      BaseStatistics[i].Min = MAX_INT64;
      BaseStatistics[i].Max = MIN_INT64;
    }
  }

  // A base statistics array exists respectively it was successfully generated
  return true;
}

bool CmFocus::addDataPoint(int64 Timestamp, int64 Value, int64 PreviousCount)
{
  // Check whether the data point belongs to current focus
  if( Timestamp < StartTime || Timestamp > EndTime )
    return false;

  // Check base statistics array
  if( !verifyBaseStatistics() )
    return false;

  // Determine interval which the data point belongs to
  int64 IntervalIndex = (Timestamp - StartTime) / IntervalLength;
  CmFocusBaseStatistics *Statistics = &BaseStatistics[IntervalIndex];

  // Check previous count
  if( Statistics->PreviousCount == 0 )
    Statistics->PreviousCount = PreviousCount;
  else
  if( Statistics->PreviousCount != PreviousCount )
    return false;

  // Incorporate a data value into appropriate interval's statistics
  {
    // Min
    if( Statistics->Min > Value )
      Statistics->Min = Value;

    // Max
    if( Statistics->Max < Value )
      Statistics->Max = Value;

    // Total
    Statistics->Total += Value;

    // Travel
    if( PreviousValue != MIN_INT64 )
      Statistics->Travel += abs(Value - PreviousValue);
    PreviousValue = Value;
  }

  // Count total number of values that were cumulated in current interval
  Statistics->Count++;

  return true;
}

CmFocusBaseStatistics * CmFocus::getFocusBaseStatistics(int32 IntervalIndex)
{
  // Check index and base statistics array
  if( IntervalIndex >= IntervalCount || BaseStatistics == NULL )
    return NULL;

  // Return requested base statistics
  return &BaseStatistics[IntervalIndex];
}


//------------------------------------------------------------------------------
// Class CmFocusBaseCascaded - Cascading base statistiscs functions
//------------------------------------------------------------------------------
CmFocusBaseCascaded::CmFocusBaseCascaded()
{


}

CmFocusBaseCascaded::~CmFocusBaseCascaded()
{
  // tbd: disconnect focus from focus list

  // Delete focus on base statistics components
  if( FocusMin )
    delete FocusMin;
  if( FocusMax )
    delete FocusMax;
  if( FocusTotal )
    delete FocusTotal;
  if( FocusTravel )
    delete FocusTravel;
}

bool CmFocusBaseCascaded::addDataInterval(int64 Timestamp, CmFocusBaseStatistics *Statistics)
{
  // Check whether the data point belongs to current focus
  if( Timestamp < StartTime || Timestamp > EndTime )
    return false;

  // Check whether the focus components are available
  if( FocusMin == NULL || FocusMax == NULL || FocusTotal == NULL || FocusTravel == NULL )
  {
    // Generate focus for base statistics components
    FocusMin = new CmFocus();
    FocusMax = new CmFocus();
    FocusTotal = new CmFocus();
    FocusTravel = new CmFocus();

    // tbd: connect focus in focus list

    // Synchronize for StartTime settings
    FocusMin->setStartTime(StartTime);
    FocusMax->setStartTime(StartTime);
    FocusTotal->setStartTime(StartTime);
    FocusTravel->setStartTime(StartTime);

    // Synchronize for EndTime settings
    FocusMin->setEndTime(EndTime);
    FocusMax->setEndTime(EndTime);
    FocusTotal->setEndTime(EndTime);
    FocusTravel->setEndTime(EndTime);

    // Synchronize for IntervalCount settings
    FocusMin->setIntervalCount(IntervalCount);
    FocusMax->setIntervalCount(IntervalCount);
    FocusTotal->setIntervalCount(IntervalCount);
    FocusTravel->setIntervalCount(IntervalCount);
  }

  // Process particular base statistics components
  {
    // Min
    FocusMin->addDataPoint(Timestamp, Statistics->Min);

    // Max
    FocusMax->addDataPoint(Timestamp, Statistics->Max);

    // Total
    FocusTotal->addDataPoint(Timestamp, Statistics->Total, Statistics->Count);

    // Travel
    FocusTravel->addDataPoint(Timestamp, Statistics->Travel, Statistics->Count);
  }

  return true;
}

