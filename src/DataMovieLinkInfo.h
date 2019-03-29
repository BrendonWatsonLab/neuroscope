#ifndef DATAMOVIELINKINFO_H
#define DATAMOVIELINKINFO_H

#include <QObject>
#include "math.h"

class DataMovieLinkInfo : public QObject
{
    Q_OBJECT
public:
    explicit DataMovieLinkInfo(QObject *parent = nullptr, const QString& movieFileUrl=QString(), const QString& dataFileUrl=QString());

    inline QString getMovieFileUrl() const { return movieFileUrl; }
    inline QString getDataFileUrl() const { return dataFileUrl; }

    // Converts a position in the data file (in MSec) to the corresponding position in the movie file (in MSec)
    qlonglong getPositionInMovieFile(qlonglong positionInDataFile);

    // Converts a position in the movie file (in MSec) to the corresponding position in the data file (in MSec)
    qlonglong getPositionInDataFile(qlonglong positionInMovieFile);

    inline qlonglong getDataOffsetToVideoMSec() const { return dataOffsetToVideoMSec; }

    inline qlonglong getDataDuration() const { return this->dataDuration; }

    inline qlonglong getVideoDuration() const { return this->videoDuration; }

    /** Takes a time in msec (representing either a duration of an offset) and brakes it down into its constituate components (12 min, 14 s, 978 ms) */
    // This section was extracted from the logic in traceview.cpp (lines 2352-2372)
    /** Returns the minute components for a duration specified in msec */
    inline static int minuteComponent(qlonglong msecDuration) { return static_cast<int>(msecDuration / static_cast<qlonglong>(60000)); }
    /** remainingRealSecondsComponent is not an integer like the other "Component" functions, and is only used for the seconds and millisecond component calculations */
    inline static qreal remainingRealSecondsComponent(qlonglong msecDuration) { return static_cast<qreal>(fmod(static_cast<qreal>(msecDuration),60000)); }
    /** Returns the remaining seconds components for a duration specified in msec after accounting for the full minutes */
    inline static int remainingSecondsComponent(qlonglong msecDuration) { return static_cast<int>(remainingRealSecondsComponent(msecDuration) / 1000); }
    /** Returns the remaining millisecond components for a duration specified in msec after accounting for the full minutes and seconds */
    //inline static int remainingMillisecondsComponent(qlonglong msecDuration) { return static_cast<int>((remainingRealSecondsComponent(msecDuration) - static_cast<qreal>(remainingSecondsComponent(msecDuration))) * 1000); }
    inline static int remainingMillisecondsComponent(qlonglong msecDuration) { return static_cast<int>(fmod(static_cast<double>(remainingRealSecondsComponent(msecDuration)),1000) + 0.5); }
    /** Takes a time in msec (representing either a duration of an offset) and returns a string representation of its constituate components (12 min, 14 s, 978 ms) */
    inline static QString formatAsComponents(qlonglong msecDuration) { return tr("%1 min %2 s %3 ms").arg(minuteComponent(msecDuration)).arg(remainingSecondsComponent(msecDuration)).arg(remainingMillisecondsComponent(msecDuration)); }


Q_SIGNALS:
    //TODO: add signals that will handle when either data file or the video are changed.

    // Copied from tracewidget.h:

    /**Informs that the starting time and/or the timeWindow have changed.
  * @param startTime starting time.
  * @param timeWindow time window.
  */
    //    void updateStartAndDuration(long startTime, long timeWindow);
    //void updateVideoDuration(qlonglong videoDuration);
    //void updateDataDuration(qlonglong dataDuration);



public Q_SLOTS:
    inline void setDataDuration(const qlonglong dataDuration) { this->dataDuration = dataDuration; }

    void setDataURL(const QString& dataFileUrl);

    void setVideoURL(const QString& movieFileUrl);

    inline void setVideoDuration(const qlonglong videoDuration) { this->videoDuration = videoDuration; }

//    /**Informs the view to present the traces for an updated time frame.*/
//    void slotStartMinuteTimeUpdated();

//    /**Informs the view to present the traces for an updated time frame.*/
//    void slotStartSecondTimeUpdated(/*int start*/);

//    /**Informs the view to present the traces for an updated time frame.*/
//    void slotStartMilisecondTimeUpdated(/*int start*/);

//    /**Update the selection widgets and informs view to present the traces for an updated time frame.*/
//    void slotDurationUpdated();

//    /**Update the selection widgets and informs view to present the traces for an updated time frame.*/
//    void slotScrollBarUpdated();


private:
    void parse();

    QString movieFileUrl;
    QString dataFileUrl;

    qlonglong dataOffsetToVideoMSec = 0;

    /**Length of the recording in miliseconds.*/
    qlonglong dataDuration = 0;
    /**Length of the recording in miliseconds.*/
    qlonglong videoDuration = 0;



};

#endif // DATAMOVIELINKINFO_H
