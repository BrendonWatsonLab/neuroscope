#include "videoplayer.h"

#include <QtWidgets>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QVideoWidget>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>
#include <QMediaMetaData>
#include "DataMovieLinkInfo.h"
#include <QShortcut>

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent)
{
    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

	// Controls:
    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    openButton->setIcon(QPixmap(":/shared-icons/document-open"));
    connect(openButton, &QAbstractButton::clicked, this, &VideoPlayer::openFile);

    // Play Button:
    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_playButton, &QAbstractButton::clicked,
            this, &VideoPlayer::play);

    // Slider
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);
    connect(m_positionSlider, &QAbstractSlider::sliderMoved,
            this, &VideoPlayer::setPosition);

    // Position Text Label
    m_positionLabel = new QLabel;
    m_positionLabel->setText("-- / -- (-- min -- s -- ms");
    m_positionLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    // Error label sits at the bottom and informs the user if there is an error loading/playing the video
    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);



    // Step Controls:
    // Step Forward:
    m_stepForward = new QPushButton;
    m_stepForward->setEnabled(false);
    m_stepForward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    connect(m_stepForward, &QAbstractButton::clicked,
            this, &VideoPlayer::stepForward);
    bindShortcut(m_stepForward, Qt::Key_Right);

    m_stepSize = new QLineEdit;
    m_stepSize->setText("1.0");
    m_stepSize->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    m_stepSize->setValidator( new QDoubleValidator(0, 100, 2, this) );
    m_stepSize->setStatusTip("Amount to step (in seconds) when the left/right arrows are pressed.");

    // Step Backward:
    m_stepBackward = new QPushButton;
    m_stepBackward->setEnabled(false);
    m_stepBackward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    connect(m_stepBackward, &QAbstractButton::clicked,
            this, &VideoPlayer::stepBackward);
    bindShortcut(m_stepBackward, Qt::Key_Left);

	// Layouts:
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    //controlLayout->addWidget(openButton);
    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider);

    QBoxLayout *secondaryControlsLayout = new QHBoxLayout;
    secondaryControlsLayout->setMargin(0);
    secondaryControlsLayout->addWidget(m_positionLabel);
    secondaryControlsLayout->addWidget(m_stepBackward);
    secondaryControlsLayout->addWidget(m_stepSize);
    secondaryControlsLayout->addWidget(m_stepForward);

    QBoxLayout *tertiaryControlsLayout = new QHBoxLayout;
    tertiaryControlsLayout->setMargin(0);
    tertiaryControlsLayout->addWidget(openButton);
    tertiaryControlsLayout->addWidget(m_errorLabel);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addLayout(controlLayout);
    layout->addLayout(secondaryControlsLayout);
    layout->addLayout(tertiaryControlsLayout);

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


void VideoPlayer::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open video file corresponding to the currently loaded data"));
    QStringList supportedMimeTypes = m_mediaPlayer->supportedMimeTypes();
//    if (!supportedMimeTypes.isEmpty())
//        fileDialog.setMimeTypeFilters(supportedMimeTypes);


    // Set the default directory to look for the movie in
    const QString directoryURL = QDir::currentPath();
    //const QString directoryURL = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath());
    //fileDialog.setDirectory(directoryURL);
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
    m_stepBackward->setEnabled(true);
    m_stepForward->setEnabled(true);
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

void VideoPlayer::stepForward() {
    QString stepSize = m_stepSize->text();
    bool wasParseSuccess = false;
    double parsedStepSize = stepSize.toDouble(&wasParseSuccess);
    if (wasParseSuccess) {
        qint64 currentPosition = this->m_mediaPlayer->position();
        // Position is in [msec]
        qint64 translatedOffset = static_cast<qint64>(parsedStepSize * 1000.0);
        qint64 updatedPosition = currentPosition + translatedOffset;
        // Pause Video Playback
        bool wasPlaying = (this->m_mediaPlayer->state() == QMediaPlayer::PlayingState);
        if (wasPlaying) {
            m_mediaPlayer->pause();
        }
        this->setPosition(updatedPosition);
        // Resume playing if we were doing so before.
        if (wasPlaying) {
            m_mediaPlayer->play();
        }
    }
    else {
        this->m_stepSize->setText("1.0");
    }
}

void VideoPlayer::stepBackward() {
    QString stepSize = m_stepSize->text();
    bool wasParseSuccess = false;
    double parsedStepSize = stepSize.toDouble(&wasParseSuccess);
    if (wasParseSuccess) {
        qint64 currentPosition = this->m_mediaPlayer->position();
        // Position is in [msec]
        qint64 translatedOffset = static_cast<qint64>(parsedStepSize * 1000.0);
        qint64 updatedPosition = currentPosition - translatedOffset;
        // Pause Video Playback
        bool wasPlaying = (this->m_mediaPlayer->state() == QMediaPlayer::PlayingState);
        if (wasPlaying) {
            m_mediaPlayer->pause();
        }
        this->setPosition(updatedPosition);
        // Resume playing if we were doing so before.
        if (wasPlaying) {
            m_mediaPlayer->play();
        }
    }
    else {
        this->m_stepSize->setText("1.0");
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
	case QMediaPlayer::EndOfMedia:
	case QMediaPlayer::BufferedMedia:
//        if (!m_mediaPlayer->isVideoAvailable()) {
//            return false;
//        }
//        return (m_mediaPlayer->duration() > 0);
        return m_mediaPlayer->isVideoAvailable();
    default:
		return false;
	}
}

//** This occurs when position is changed due to media playback (via the play button) or something similar */
void VideoPlayer::positionChanged(qint64 position)
{
    // If the slider value differs from the media player position, we know the slider is out of date
    bool positionChanged = (position != m_positionSlider->value());
    if (positionChanged) {
        m_positionSlider->setValue(position);
        //emit onPositionOrActiveWindowChanged(position, this->activeWindowDuration);
    }
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
    m_stepBackward->setEnabled(false);
    m_stepForward->setEnabled(false);
    const QString errorString = m_mediaPlayer->errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(m_mediaPlayer->error()));
    else
        message += errorString;

    m_errorLabel->setText(message);
}


// I have this function in my 'utils' module.
void VideoPlayer::bindShortcut(QAbstractButton *button, const QKeySequence &shortcut)
{
    QObject::connect(new QShortcut(shortcut, button), &QShortcut::activated, [button](){ button->animateClick(); });
}


void VideoPlayer::refreshThumbnail() {
    switch(this->m_mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        return; // If we're already playing, there is no need to update the thumbnail
    default:
        // Play and then immediately pause to refresh the thumbnail
        this->play();
        this->play();
        break;
    }
}
