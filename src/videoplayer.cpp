#include "videoplayer.h"

#include <QtWidgets>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QVideoWidget>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>
#include <QMediaMetaData>
#include "DataMovieLinkInfo.h"

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent)
{
    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

	// Controls:
    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, &QAbstractButton::clicked, this, &VideoPlayer::openFile);

    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(m_playButton, &QAbstractButton::clicked,
            this, &VideoPlayer::play);

    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);

    connect(m_positionSlider, &QAbstractSlider::sliderMoved,
            this, &VideoPlayer::setPosition);


    m_positionLabel = new QLabel;
    m_positionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    m_syncDataPositionFromVideoWindowButton = new QPushButton(tr("Video --> Data"));
    m_syncDataPositionFromVideoWindowButton->setEnabled(false);
    connect(m_syncDataPositionFromVideoWindowButton, &QAbstractButton::clicked, this, &VideoPlayer::setDataFilePositionFromPosition);

    m_syncVideoPositionFromDataWindowButton = new QPushButton(tr("Data --> Video"));
    m_syncVideoPositionFromDataWindowButton->setEnabled(false);
    connect(m_syncVideoPositionFromDataWindowButton, &QAbstractButton::clicked, this, &VideoPlayer::getPositionFromDataFile);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	// Layouts:
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    //controlLayout->addWidget(openButton);
    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider);

    QBoxLayout *secondaryControlsLayout = new QHBoxLayout;
    secondaryControlsLayout->setMargin(0);
    secondaryControlsLayout->addWidget(openButton);
    secondaryControlsLayout->addWidget(m_positionLabel);
    secondaryControlsLayout->addWidget(m_syncDataPositionFromVideoWindowButton);
    secondaryControlsLayout->addWidget(m_syncVideoPositionFromDataWindowButton);


    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addLayout(controlLayout);
    layout->addLayout(secondaryControlsLayout);
    layout->addWidget(m_errorLabel);

    setLayout(layout);

	// Connect MediaPlayer Signals:
    m_mediaPlayer->setVideoOutput(videoWidget);
    connect(m_mediaPlayer, &QMediaPlayer::stateChanged, this, &VideoPlayer::mediaStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::mediaChanged, this, &VideoPlayer::currentMediaChanged);
    connect(m_mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::durationChanged);
    connect(m_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &VideoPlayer::handleError);


    printPlayableFileFormats();
}

VideoPlayer::~VideoPlayer()
{
}


void VideoPlayer::getPositionFromDataFile() {

}

void VideoPlayer::setDataFilePositionFromPosition() {

}

void VideoPlayer::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Movie"));
    QStringList supportedMimeTypes = m_mediaPlayer->supportedMimeTypes();
//    if (!supportedMimeTypes.isEmpty())
//        fileDialog.setMimeTypeFilters(supportedMimeTypes);


    // Set the default directory to look for the movie in
    const QString directoryURL = QDir::currentPath();
    //const QString directoryURL = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath());
    fileDialog.setDirectory(directoryURL);
    if (fileDialog.exec() == QDialog::Accepted)
        setUrl(fileDialog.selectedUrls().constFirst());
}

QUrl VideoPlayer::getUrl()
{
    return m_mediaPlayer->media().canonicalUrl();
}

void VideoPlayer::setUrl(const QUrl &url)
{
    qInfo(qUtf8Printable(url.toDisplayString(QUrl::PreferLocalFile)));
    m_errorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    m_mediaPlayer->setMedia(url);
    m_playButton->setEnabled(true);
}

void VideoPlayer::play()
{
    switch (m_mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        m_mediaPlayer->pause();
        break;
    default:
        m_mediaPlayer->play();
        break;
    }
}

void VideoPlayer::currentMediaChanged(const QMediaContent &media) {
    emit onMediaChanged(media);
    emit onMediaUrlChanged(media.canonicalUrl().toDisplayString());
}


void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::LoadedMedia) {
        this->videoFrameRate = m_mediaPlayer->metaData(QMediaMetaData::VideoFrameRate).value<qreal>();
    }
}

void VideoPlayer::getMetaData() {
    // Get the list of keys there is metadata available for
    QStringList metadatalist = m_mediaPlayer->availableMetaData();

    // Get the size of the list
    int list_size = metadatalist.size();

    //qDebug() << player->isMetaDataAvailable() << list_size;

    // Define variables to store metadata key and value
    QString metadata_key;
    QVariant var_data;

    for (int indx = 0; indx < list_size; indx++)
    {
        // Get the key from the list
        metadata_key = metadatalist.at(indx);

        // Get the value for the key
        var_data = m_mediaPlayer->metaData(metadata_key);

        qDebug() << metadata_key << var_data.toString();
    }



}

bool VideoPlayer::hasValidVideo()
{	
    const QMediaPlayer::MediaStatus status = m_mediaPlayer->mediaStatus();
	switch (status) {
	case QMediaPlayer::LoadedMedia:
		return m_mediaPlayer->isVideoAvailable();
	case QMediaPlayer::EndOfMedia:
		return m_mediaPlayer->isVideoAvailable();
	case QMediaPlayer::BufferedMedia:
		return m_mediaPlayer->isVideoAvailable();
	default:
		return false;
	}
}

//** This occurs when position is changed due to media playback (via the play button) or something similar */
void VideoPlayer::positionChanged(qint64 position)
{
    m_positionSlider->setValue(position);
    this->updatePositionDurationLabel();
}

void VideoPlayer::durationChanged(qint64 duration)
{
    m_positionSlider->setRange(0, duration);
    this->updatePositionDurationLabel();
    emit onDurationChanged(duration);
}

qint64 VideoPlayer::getDuration()
{
    return m_mediaPlayer->duration();
}

qint64 VideoPlayer::getPosition()
{
    return m_mediaPlayer->position();
}

qreal VideoPlayer::getFramesPerSecond() {
    return this->videoFrameRate;
}

void VideoPlayer::printPlayableFileFormats()
{
    QStringList supportedMimeTypes = m_mediaPlayer->supportedMimeTypes();
    QString outputString = "supportedMimeTypes: ";
    for(int i=0 ; i < supportedMimeTypes.length() ; i++)
    {
        outputString.append(supportedMimeTypes.at(i));
        outputString.append(", ");
    }

    qInfo(qUtf8Printable(outputString));
}

/* This called when the slider is adjusted */
void VideoPlayer::setPosition(qlonglong position)
{
    this->setPositionAndActiveWindow(position, this->activeWindowDuration);
}

/* This is called when the slider is adjusted or moved in correspondence to a synchronized data view */
void VideoPlayer::setPositionAndActiveWindow(qlonglong position, qlonglong activeWindow) {
    bool positionChanged = (position != m_mediaPlayer->position());
    bool activeWindowDurationChanged = (activeWindow != this->activeWindowDuration);

    if (activeWindowDurationChanged) {
        this->activeWindowDuration = activeWindow;
    }
    if (positionChanged) {
        m_mediaPlayer->setPosition(position);
        this->updatePositionDurationLabel();
    }
    if (positionChanged || activeWindowDurationChanged) {
        emit onPositionOrActiveWindowChanged(position, activeWindow);
    }

}


void VideoPlayer::updatePositionDurationLabel() {
    const qint64 currPosition = this->getPosition();
    const qint64 currDuration = this->getDuration();

    const QString strPosition = QString::number(currPosition); // actual conversion
    const QString strDuration = QString::number(currDuration); // actual conversion
    const QString formattedComponentsPosition = DataMovieLinkInfo::formatAsComponents(currPosition);

    const QString finalOutputString = strPosition + " / " + strDuration + " (" + formattedComponentsPosition + ")";
    this->m_positionLabel->setText(finalOutputString);
}

void VideoPlayer::handleError()
{
    m_playButton->setEnabled(false);
    const QString errorString = m_mediaPlayer->errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(m_mediaPlayer->error()));
    else
        message += errorString;

    m_errorLabel->setText(message);
}
