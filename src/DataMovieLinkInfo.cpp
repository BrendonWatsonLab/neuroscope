#include "DataMovieLinkInfo.h"
#include <QDate>
#include <QTime>
#include <QDir>

//DataMovieLinkInfo.cpp
DataMovieLinkInfo::DataMovieLinkInfo(QObject *parent, const QString& movieFileUrl, const QString& dataFileUrl) : QObject(parent), movieFileUrl(movieFileUrl), dataFileUrl(dataFileUrl), dataOffsetToVideoMSec(0)
{
    this->parse();

    //connect(&view,SIGNAL(setStartAndDuration(long,long)),this, SLOT(slotSetStartAndDuration(long,long)));

}

void DataMovieLinkInfo::parse()
{
    // Video File:
    const QStringList videoFilePathParts = movieFileUrl.split(QDir::separator(), QString::SkipEmptyParts);
    const QString videoFileName = videoFilePathParts.last();
    const QStringList videoFileNameComponents = videoFileName.split(QLatin1Char('.'), QString::SkipEmptyParts);
    const QString videoFileBaseName = videoFileNameComponents.first();
    QStringList videoFileBaseNameParts = videoFileBaseName.split(QLatin1Char('_'), QString::SkipEmptyParts);
    // Jasper_20190319_125207822.avi
    QDate videoFileDate = QDate::fromString(videoFileBaseNameParts[1],"yyyyMMdd");
    QTime videoFileTime = QTime::fromString(videoFileBaseNameParts[2], "hhmmsszzz");

    // Data File:

    // Perform the diff
    //qlonglong dataRecordingLength = getDocument()->recordingLength();
    qInfo(qUtf8Printable(dataFileUrl));
    QStringList dataFilePathParts = dataFileUrl.split(QDir::separator(), QString::SkipEmptyParts);
    int parentDirectoryIndex = dataFilePathParts.count() - 2;
    QString parentDirectoryName = dataFilePathParts[parentDirectoryIndex];
    QStringList parentDirectoryNameParts = parentDirectoryName.split(QLatin1Char('_'), QString::SkipEmptyParts);

    // Jasper_190319_125141
    //QString date_string_on_db = parentDirectoryNameParts[1] + "T" + parentDirectoryNameParts[2];
    QDate dataFileDate = QDate::fromString(parentDirectoryNameParts[1],"yyMMdd");
    QTime dataFileTime = QTime::fromString(parentDirectoryNameParts[2], "hhmmss");


    // Video Lag Time:
    this->dataOffsetToVideoMSec = dataFileTime.msecsTo(videoFileTime);
    QString printable = QStringLiteral("Offset to video: %1 [msec].").arg(this->dataOffsetToVideoMSec);
    qInfo(qUtf8Printable(printable));

}


// Converts a position in the data file (in MSec) to the corresponding position in the movie file (in MSec)
qlonglong DataMovieLinkInfo::getPositionInMovieFile(qlonglong positionInDataFile) {
    //TODO: Constrain between 0 and duration of movie file
    return positionInDataFile + this->dataOffsetToVideoMSec;
}

// Converts a position in the movie file (in MSec) to the corresponding position in the data file (in MSec)
qlonglong DataMovieLinkInfo::getPositionInDataFile(qlonglong positionInMovieFile) {
    //TODO: Constrain between 0 and duration of data file
    return positionInMovieFile - this->dataOffsetToVideoMSec;
}
