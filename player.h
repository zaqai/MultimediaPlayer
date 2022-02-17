#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include"videowidget.h"

class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QVideoProbe;
class QVideoWidget;
class QAudioProbe;


class PlaylistModel;

class Player : public QWidget
{
    Q_OBJECT

public:

    explicit Player(QWidget *parent = nullptr);
    ~Player();
    void sendSlot();//点击quit按钮，调用此槽函数，发送 mySignal
    bool isPlayerAvailable() const;

    void addToPlaylist(const QList<QUrl> &urls);
    void setCustomAudioRole(const QString &role);

signals:
    void fullScreenChanged(bool fullScreen);
    void mySignal();

private slots:
    void open();
    void durationChanged(qint64 duration);      //实现进度条自动增长
    void positionChanged(qint64 progress);
    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);
    void videoAvailableChanged(bool available);

private:
    void updateDurationInfo(qint64 currentInfo);
    QMediaPlayer *m_player = nullptr;
    QMediaPlaylist *m_playlist = nullptr;
    VideoWidget *m_videoWidget = nullptr;
    QSlider *m_slider = nullptr;
    QLabel *m_labelDuration = nullptr;
    QPushButton *m_fullScreenButton = nullptr;
    QPushButton *m_quitButton = nullptr;
    QVideoProbe *m_videoProbe = nullptr;
    QAudioProbe *m_audioProbe = nullptr;
    PlaylistModel *m_playlistModel = nullptr;
    QAbstractItemView *m_playlistView = nullptr;
    qint64 m_duration;
};

