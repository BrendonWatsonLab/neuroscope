#ifndef DATAMOVIELINKINFO_H
#define DATAMOVIELINKINFO_H

#include <QObject>

class DataMovieLinkInfo : public QObject
{
    Q_OBJECT
public:
    explicit DataMovieLinkInfo(QObject *parent = nullptr, const QString& movieFileUrl=QString(), const QString& dataFileUrl=QString());

    QString movieFileUrl;
    QString dataFileUrl;

    // Converts a position in the data file (in MSec) to the corresponding position in the movie file (in MSec)
    qlonglong getPositionInMovieFile(qlonglong positionInDataFile);

    // Converts a position in the movie file (in MSec) to the corresponding position in the data file (in MSec)
    qlonglong getPositionInDataFile(qlonglong positionInMovieFile);

    inline qlonglong getDataOffsetToVideoMSec() const { return dataOffsetToVideoMSec; }

    inline qlonglong getDataDuration() const { return this->dataDuration; }
    inline void setDataDuration(const qlonglong dataDuration) { this->dataDuration = dataDuration; }
    inline qlonglong getVideoDuration() const { return this->videoDuration; }
    inline void setVideoDuration(const qlonglong videoDuration) { this->videoDuration = videoDuration; }


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

    qlonglong dataOffsetToVideoMSec = 0;

    /**Length of the recording in miliseconds.*/
    qlonglong dataDuration = 0;
    /**Length of the recording in miliseconds.*/
    qlonglong videoDuration = 0;

};

#endif // DATAMOVIELINKINFO_H
