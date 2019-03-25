#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>
#include <QWidget>
#include <QtGui/QMovie>

class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;

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

    public slots:
        void openFile();
        void play();

    private slots:
        void mediaStateChanged(QMediaPlayer::State state);
        void positionChanged(qint64 position);
        void durationChanged(qint64 duration);
        void setPosition(int position);
        void handleError();

    private:
        QMediaPlayer* m_mediaPlayer;
        QAbstractButton *m_playButton;
        QSlider *m_positionSlider;
        QLabel *m_errorLabel;


};

#endif // VIDEOPLAYER_H
