#include <QMediaPlayer>
#include <QWidget>


class QAbstractButton;
class QAbstractSlider;
class QComboBox;


class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) ;
    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;//静音
    qreal playbackRate() const;//倍速

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);
    void playClicked();

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:

    void muteClicked();
    void updateRate();
    void onVolumeSliderValueChanged();

private:
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    bool m_playerMuted = false;
    QAbstractButton *m_playButton = nullptr;
    QAbstractButton *m_stopButton = nullptr;
    QAbstractButton *m_nextButton = nullptr;
    QAbstractButton *m_previousButton = nullptr;
    QAbstractButton *m_muteButton = nullptr;
    QAbstractSlider *m_volumeSlider = nullptr;
    QComboBox *m_rateBox = nullptr;
};


