//////////////////////////////////////////////////////////////////////////////
//
// CmFocus.h: Declaration of CmFocus classes
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


#ifndef CmFocusH
#define CmFocusH

#include "Services/Basic/CmTime.h"

namespace Cosmos
{

// Base focus class statistics
typedef struct
{
  int64 Min;               // minimal value
  int64 Max;               // maximal value
  int64 Total;             // total value of all samples
  int64 Travel;            // travel value of all samples
  int64 Count;             // count inside this processing stage
  int64 PreviousCount;     // count in a previous processing stage
} CmFocusBaseStatistics;

/**
 *  CmFocus.
 *  The CmFocus class provides for the ability to set a focus on a data object.
 *
 *  A data object in the scope of this class is any data source that delivers
 *  one- or multi-dimensional data streams which data points can be uniquely
 *  assigned to a timestamp.
 *
 *  A focus is understood in the scope of this class as a data reduction from
 *  the input data stream to output data. The data reduction can be done by
 *  replacing a group of input data points e.g. by statistical values like an
 *  average of those input data points. Generally, there can be any rule applied
 *  for data reduction including the possibility to ignore data points from the
 *  input data stream.
 *
 *  The focus class further provides for a list of contained focus classes. This
 *  allows to generate a hierarchical structur of focus classes for presenting
 *  data streams in an as flexible as possible way.
 *
 *  The top focus class in the hierarchy of muliple focus classes will usually
 *  be a display focus class. That kind of focus class will generate as output
 *  a device independent bitmap which is transferred to screen for finally
 *  presenting the input data to users in a suitable form.
 *
 *  Since the so defined focus is the way of looking on a data set there might
 *  be developed a lot of focus implementations in future, as many as there
 *  exist different ways of looking on a set of data. As a default and basic
 *  strategy the following implementation applies to the CmFocus base class.
 *  As usual, the default behavior can be overwritten by implementing the
 *  processing function in a different way. However, the default implementation
 *  works as follows:
 *
 *  1) A focus will have a start time, an end time and an interval length
 *     assigned. Data points will be accepted only for the time from start time
 *     to end time and further characteristics will be calculated for the
 *     intervals between start and end time.
 *
 *  2) A count of data points will be determined for each interval separately.
 *
 *  3) The minimal, maximal, total and travel values will be calculated for
 *     each interval separately. The minimal and maximal values can be queried
 *     for each interval directly. For an average value, the total value of all
 *     data points will be divided by the count of data points inside an interval.
 *
 *  4) The travel value summarizes the differences between two adjacent data
 *     points for each interval separately, thereby also taking the difference
 *     between the last data point in an interval and the first data point in a
 *     subsequent interval into account. When queried, the sum of all differences
 *     will be devided by the number of data points inside an interval.
 */
class CmFocus
{
//public:
//  // Features
//  CmPixel ChartColor;            // color that will be used for charting
//  CmPixel ChartBackground;       // color that will be used for background
//
public:
  /** setStartTime.
   *  The focus' start time will be set.
   */
  void setStartTime(int64 StartTime);

  /** setEndTime.
   *  The focus' end time will be set.
   */
  void setEndTime(int64 EndTime);

  /** setIntervalLength.
   *  The focus' interval length will be set and the number of intervals will
   *  be returned.
   */
  int64 setIntervalLength(int64 IntervalLength);

  /** setIntervalCount.
   *  The focus' number of intervals will be set and the length of an interval
   *  will be returned.
   */
  int64 setIntervalCount(int64 IntervalCount);

  /** Get time settings.
   *  Particular time settings will be returned
   */
  int64 getStartTime();
  int64 getEndTime();
  int64 getIntervalLength();
  int64 getIntervalCount();

  /** clearBaseStatistics.
   *  The base statistics array will be cleared.
   */
  void clearBaseStatistics();

  /** verifyBaseStatistics.
   *  The base statistics array will be veryfied if it exists already. If this
   *  is not the case it will be created.
   */
  bool verifyBaseStatistics(); 

  /** addDataPoint.
   *  A data point will be investigated whether it belongs to current focus and
   *  if yes then it will be incorporated into base statistics. The function
   *  returns true if the data point was used, otherwise it returns false.
   *
   *  The previous count determines the number of values that were cumulated
   *  in a previous processing stage.
   */
  bool addDataPoint(int64 Timestamp, int64 Value, int64 PreviousCount =1);

  /** getFocusValue.
   *  A structure with base statistics will be returned for a given IntervalIndex.
   */
  CmFocusBaseStatistics * getFocusBaseStatistics(int32 IntervalIndex);

public:
  // Constructors and desctructor
  CmFocus();
  ~CmFocus();

private:
  // List of contained focus classes
  CmFocus *FocusList;

  // Base class statistics
  CmFocusBaseStatistics *BaseStatistics;
  int64 PreviousValue;                       // for calculating travel values

friend class CmFocusBaseCascaded;

protected:
  // Time settings
  int64  StartTime;
  int64  EndTime;
  int64  IntervalLength;
  int64  IntervalCount;
};

/**
 *  CmFocusBaseCascaded.
 *  When a focus is applied on the output of another focus the expectation is
 *  that the output of the last focus in a chain will be equal or at least that
 *  it will be very close to the output of a single focus operating directly on
 *  the measurement data. In order to meet that expectation, a base statistics
 *  focus is applied on each of the components of the first focus in the chain.
 *
 *  For example, when a focus operates on sample data blocks and determines the
 *  maximal value in each sample data block then the set of maximal values will
 *  itself become a data block with highs, lows, an average value and a travel
 *  value. Thus running a secondary focus on the maximal value of a primary
 *  focus will preserve the statistics of the set of maximal values.
 *
 *  For performance reasons it is desirable to operate on integers. In order to
 *  meet this goal a total is calculated along with a count of the values that
 *  went into the total value. Though on a single focus the situation is rather
 *  transparent it gets more complex when running a focus on each of the focus
 *  components. From the mixture of base values and cumulated values there will
 *  result the following matrix for the right count to be aplied for each value:
 *
 *  Input value  |  Input count  |           Output count
 *               |               |      Min/Max       Total/Travel
 *  ------------------------------------------------------------------
 *  Min/Max      |      1        |         1            Count
 *  Total/Travel | PreviousCount |  PreviousCount  PreviousCount*Count
 *  ------------------------------------------------------------------
 *
 *  Thus the Count and PreviousCount values have to be stored along with other
 *  values, separately for each interval. Furthermore, only values with the
 *  same PreviousCount may be cumulated in the second focus stage.
 */
class CmFocusBaseCascaded : public CmFocus
{
public:
  /** addDataInterval.
   *  A data interval will be investigated whether it belongs to current focus
   *  and if yes then each base statistics component will be incorporated into
   *  base statistics of a separate focus. The function returns true if the
   *  data interval was used, otherwise it returns false.
   */
  bool addDataInterval(int64 Timestamp, CmFocusBaseStatistics *Statistics);


public:
  // Constructor and destructor
  CmFocusBaseCascaded();
  ~CmFocusBaseCascaded();

private:
  // Focus on base statistics components
  // tbd: take also the total count into account
  CmFocus *FocusMin;
  CmFocus *FocusMax;
  CmFocus *FocusTotal;
  CmFocus *FocusTravel;

};







/**
 *  CmCanvas.
 *  The CmCanvas provides for RGB drawing capabilities. It offers painting
 *  functions for basic geometric objects like a dot, a line, a circle and more.
 *  The implementation of a canvas is done as a two-dimensional array of 32-bit
 *  color values in memory. That memory can be transfered to screen, written
 *  into an image file on disk or further processed as image data.
 */
class CmFocusCanvas : public CmFocus
{
public:
	// Access parameters;
	void setSize(int32 nImgWidth,int32 nImgHeight);
	int32 getImgWidth();
	int32 getImgHeight();

	// Constructor and destructor
	CmFocusCanvas();
	~CmFocusCanvas();

protected:
  // Image parameters
	int32	ImgWidth;
	int32	ImgHeight;


private:

};




}  // namespace Cosmos

#endif
