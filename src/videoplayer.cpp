#include "videoplayer.h"

#include <QtWidgets>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QVideoWidget>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent)
{
    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
       QVideoWidget *videoWidget = new QVideoWidget;

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

       m_errorLabel = new QLabel;
       m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

       QBoxLayout *controlLayout = new QHBoxLayout;
       controlLayout->setMargin(0);
       controlLayout->addWidget(openButton);
       controlLayout->addWidget(m_playButton);
       controlLayout->addWidget(m_positionSlider);

       QBoxLayout *layout = new QVBoxLayout;
       layout->addWidget(videoWidget);
       layout->addLayout(controlLayout);
       layout->addWidget(m_errorLabel);

       setLayout(layout);

       m_mediaPlayer->setVideoOutput(videoWidget);
       connect(m_mediaPlayer, &QMediaPlayer::stateChanged, this, &VideoPlayer::mediaStateChanged);
       connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);
       connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::durationChanged);
       connect(m_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &VideoPlayer::handleError);
}

VideoPlayer::~VideoPlayer()
{
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

void VideoPlayer::positionChanged(qint64 position)
{
    m_positionSlider->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    m_positionSlider->setRange(0, duration);
}

qint64 VideoPlayer::getDuration()
{
    return m_mediaPlayer->duration();
}

qint64 VideoPlayer::getPosition()
{
    return m_mediaPlayer->position();
}

void VideoPlayer::setPosition(int position)
{
    m_mediaPlayer->setPosition(position);
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
