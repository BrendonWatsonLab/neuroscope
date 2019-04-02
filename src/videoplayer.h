#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>
#include <QWidget>
#include <QtGui/QMovie>

class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
class QLineEdit;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

        QUrl getUrl();
        void setUrl(const QUrl &url);
        qint64 getDuration();
        qint64 getPosition();
        qreal getFramesPerSecond();
		void printPlayableFileFormats();
        void getMetaData();
		bool hasValidVideo();

    public slots:
        void openFile();
        void play();
        void stepForward();
        void stepBackward();

        void setPosition(qlonglong position);
        void setPositionAndActiveWindow(qlonglong position, qlonglong activeWindow);

        void getPositionFromDataFile();
        void setDataFilePositionFromPosition();


    signals:
        void onPositionOrActiveWindowChanged(qint64 newPosition, qint64 newActiveWindow);
        //void onPositionChanged(qint64 newPosition);
        void onDurationChanged(qint64 newDuration);
        void onMediaChanged(const QMediaContent& media);
        void onMediaUrlChanged(const QString& newMediaUrl);

    private slots:
        void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
        void mediaStateChanged(QMediaPlayer::State state);
        void currentMediaChanged(const QMediaContent &media);
        void positionChanged(qint64 position);
        void durationChanged(qint64 duration);
        void handleError();

    private:
        QMediaPlayer* m_mediaPlayer;
        QAbstractButton *m_playButton;
        QSlider *m_positionSlider;
        QLabel *m_errorLabel;
        QLabel *m_positionLabel;

        QAbstractButton *m_syncDataPositionFromVideoWindowButton;
        QAbstractButton *m_syncVideoPositionFromDataWindowButton;

        QAbstractButton *m_stepForward;
        QLineEdit *m_stepSize;
        QAbstractButton *m_stepBackward;

        qreal videoFrameRate = 0;
        qint64 activeWindowDuration = 1000;

        void updatePositionDurationLabel();
        void bindShortcut(QAbstractButton *button, const QKeySequence &shortcut);

};

#endif // VIDEOPLAYER_H
