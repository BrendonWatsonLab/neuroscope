#include "DataMovieLinkInfo.h"
#include <QtGlobal>
#include <QDate>
#include <QTime>
#include <QDir>

#include "math.h"

//DataMovieLinkInfo.cpp
DataMovieLinkInfo::DataMovieLinkInfo(QObject *parent, const QString& movieFileUrl, const QString& dataFileUrl) : QObject(parent), movieFileUrl(movieFileUrl), dataFileUrl(dataFileUrl), dataOffsetToVideoMSec(0)
{
    this->parse();

    //connect(&view,SIGNAL(setStartAndDuration(long,long)),this, SLOT(slotSetStartAndDuration(long,long)));

}


void DataMovieLinkInfo::setDataURL(const QString& dataFileUrl) {
    if (dataFileUrl != this->dataFileUrl) {
        this->dataFileUrl = dataFileUrl;
        this->parse();
        //emit valueChanged(dataFileUrl);
    }
}

void DataMovieLinkInfo::setVideoURL(const QString& movieFileUrl) {
    if (movieFileUrl != this->movieFileUrl) {
        this->movieFileUrl = movieFileUrl;
        this->parse();
        //emit valueChanged(movieFileUrl);
    }
}

void DataMovieLinkInfo::parse()
{
    //NOTE: should be forward slash even on windows!
        // DataFileURL: C:/Users/watsonlab/Desktop/VideoSync/Jasper_190319_125141/amplifier_analogin_auxiliary_int16.dat
        // MovieFileURL: file:///C:/Users/watsonlab/Desktop/VideoSync/Jasper_20190319_125207822.m4v
    //QChar separatorCharacter = QDir::separator();
    QChar separatorCharacter = '/';

    // Video File:
    const QStringList videoFilePathParts = movieFileUrl.split(separatorCharacter, QString::SkipEmptyParts);
    const QString videoFileName = videoFilePathParts.last();
    const QStringList videoFileNameComponents = videoFileName.split(QLatin1Char('.'), QString::SkipEmptyParts);
    const QString videoFileBaseName = videoFileNameComponents.first();
    QStringList videoFileBaseNameParts = videoFileBaseName.split(QLatin1Char('_'), QString::SkipEmptyParts);
    // Jasper_20190319_125207822.avi
    QDate videoFileDate = QDate::fromString(videoFileBaseNameParts[1],"yyyyMMdd");
    QTime videoFileTime = QTime::fromString(videoFileBaseNameParts[2], "hhmmsszzz");

    // Data File:

    // Perform the diff
    qInfo(qUtf8Printable(dataFileUrl));

    QStringList dataFilePathParts = dataFileUrl.split(separatorCharacter, QString::SkipEmptyParts);
    int parentDirectoryIndex = dataFilePathParts.count() - 2;
    QString parentDirectoryName = dataFilePathParts[parentDirectoryIndex];
    QStringList parentDirectoryNameParts = parentDirectoryName.split(QLatin1Char('_'), QString::SkipEmptyParts);

    // Jasper_190319_125141
    QDate dataFileDate = QDate::fromString(parentDirectoryNameParts[1],"yyMMdd");
    QTime dataFileTime = QTime::fromString(parentDirectoryNameParts[2], "hhmmss");

    // Video Lag Time:
    this->dataOffsetToVideoMSec = dataFileTime.msecsTo(videoFileTime);

    QString printable = QStringLiteral("Offset to video: %1 [msec] - %2").arg(this->dataOffsetToVideoMSec).arg(DataMovieLinkInfo::formatAsComponents(this->dataOffsetToVideoMSec));
    qInfo(qUtf8Printable(printable));

}


// Converts a position in the data file (in MSec) to the corresponding position in the movie file (in MSec)
qlonglong DataMovieLinkInfo::getPositionInMovieFile(qlonglong positionInDataFile) {
    //TODO: Good Constraints, but are you certain we're supposed to be adding (as opposed to subtracting), etc)? Test the logic.
    return qBound(static_cast<qlonglong>(0), (positionInDataFile + this->dataOffsetToVideoMSec), this->getVideoDuration());
}

// Converts a position in the movie file (in MSec) to the corresponding position in the data file (in MSec)
qlonglong DataMovieLinkInfo::getPositionInDataFile(qlonglong positionInMovieFile) {
    //TODO: Constrain between 0 and duration of data file
    return qBound(static_cast<qlonglong>(0), (positionInMovieFile - this->dataOffsetToVideoMSec), this->getDataDuration());
}

// Triggered when the user adjusts the position in the data window. Translates the updated position to the video window and sends out the signal
void DataMovieLinkInfo::slotDataOffsetAndWindowUpdated(qlonglong startTime, qlonglong timeWindow) {
    // If we want to sync to the start of the window (which we assume we do), add the offset
    qlonglong moviePosition = this->getPositionInMovieFile(startTime);
    // Re-emit the new event for reception by the video player.
    emit updateOffsetAndWindowFromData(moviePosition, timeWindow);
}

// Triggered when the user adjusts the position in the video window. Translates the updated position to the data window and sends out the signal
void DataMovieLinkInfo::slotVideoOffsetAndWindowUpdated(qlonglong startTime, qlonglong timeWindow) {
    // If we want to sync to the start of the window (which we assume we do), add the offset
    qlonglong dataPosition = this->getPositionInDataFile(startTime);
    // Re-emit the new event for reception by the video player.
    emit updateOffsetAndWindowFromVideo(dataPosition, timeWindow);
}



