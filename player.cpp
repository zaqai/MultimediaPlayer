#include "player.h"

#include "playercontrols.h"
#include "playlistmodel.h"

#include<QDebug>
#include <QMediaPlaylist>
#include <QVideoProbe>          //帧
#include <QAudioProbe>          //帧
#include <QtWidgets>

Player::Player(QWidget *parent)
    : QWidget(parent)
{
    this->resize(800,600);
    setWindowTitle("videoPlayer");
    m_player = new QMediaPlayer(this);
    m_player->setAudioRole(QAudio::VideoRole);

    m_playlist = new QMediaPlaylist();
    m_player->setPlaylist(m_playlist);


    connect(m_player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);//实现进度条拖动
    connect(m_player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);//实现进度信息动态增长
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);//当前激活的播放列表项目
    connect(m_player, &QMediaPlayer::videoAvailableChanged, this, &Player::videoAvailableChanged);//检测文件是否可读取
    m_videoWidget = new VideoWidget(this);
    m_player->setVideoOutput(m_videoWidget);
    m_playlistModel = new PlaylistModel(this);
    m_playlistModel->setPlaylist(m_playlist);
    m_playlistView = new QListView(this);
    m_playlistView->setModel(m_playlistModel);
    m_playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));
    connect(m_playlistView, &QAbstractItemView::activated, this, &Player::jump);
    //双击播放列表项目播放
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, m_player->duration() / 1000);

    m_labelDuration = new QLabel(this);
    connect(m_slider, &QSlider::sliderMoved, this, &Player::seek);


    m_videoProbe = new QVideoProbe(this);

    m_videoProbe->setSource(m_player);

    m_audioProbe = new QAudioProbe(this);

    m_audioProbe->setSource(m_player);

    QPushButton *openButton = new QPushButton( this);
    openButton->setIcon(QIcon("://icon/open.png"));
    openButton->setIconSize(QSize(50,50));
    QPushButton *m_quitButton =new QPushButton(this);
    m_quitButton->setIcon(QIcon("://icon/close.png"));
    m_quitButton->setIconSize(QSize(50,50));
    connect(openButton, &QPushButton::clicked, this, &Player::open);

    connect(m_quitButton, &QPushButton::clicked, this, &Player::sendSlot);
    connect(m_quitButton, &QPushButton::clicked, this, &QWidget::close);
    PlayerControls *controls = new PlayerControls(this);
    controls->setState(m_player->state());
    controls->setVolume(m_player->volume());
    controls->setMuted(controls->isMuted());

    connect(m_videoWidget,&VideoWidget::pressSignal,controls,&PlayerControls::playClicked);

    connect(controls, &PlayerControls::play, m_player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, m_player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, m_player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::next, m_playlist, &QMediaPlaylist::next);
    connect(controls, &PlayerControls::previous,m_playlist ,&QMediaPlaylist::previous );
    connect(controls, &PlayerControls::changeVolume, m_player, &QMediaPlayer::setVolume);
    connect(controls, &PlayerControls::changeMuting, m_player, &QMediaPlayer::setMuted);
    connect(controls, &PlayerControls::changeRate, m_player, &QMediaPlayer::setPlaybackRate);


    connect(m_player, &QMediaPlayer::stateChanged, controls, &PlayerControls::setState);
    connect(m_player, &QMediaPlayer::volumeChanged, controls, &PlayerControls::setVolume);
    connect(m_player, &QMediaPlayer::mutedChanged, controls, &PlayerControls::setMuted);

    m_fullScreenButton = new QPushButton( this);
    m_fullScreenButton->setIcon(QIcon("://icon/fullscreen.png"));
    m_fullScreenButton->setIconSize(QSize(50,50));
    m_fullScreenButton->setCheckable(true);

    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(m_videoWidget, 2);
    displayLayout->addWidget(m_playlistView);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);
    controlLayout->addWidget(m_fullScreenButton);
    controlLayout->addWidget(m_quitButton);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_slider);
    hLayout->addWidget(m_labelDuration);
    layout->addLayout(hLayout);
    layout->addLayout(controlLayout);

    setLayout(layout);

    if (!isPlayerAvailable())
    {
        controls->setEnabled(false);
        m_playlistView->setEnabled(false);
        openButton->setEnabled(false);

        m_fullScreenButton->setEnabled(false);
    }

}

Player::~Player()
{
}

void Player::sendSlot()
{

    emit mySignal();
}

bool Player::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void Player::open()
{
    QFileDialog fileDialog(this);
    //    fileDialog.setWindowTitle("打开文件");
    //    QStringList filters;
    //    QString fil= "视频文件 (*.mp4 *.mkv *.avi *.flv)";
    //    fileDialog.setNameFilters(filters);
    QList<QUrl> urls=fileDialog.getOpenFileUrls(this,"打开文件",tr(""),"*.mp4 *.avi *.mkv *.wmv *.mp3 *.wma *.flac *.wav");
    addToPlaylist(urls);
}


void Player::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        m_playlist->addMedia(url);
    }
}

void Player::setCustomAudioRole(const QString &role)
{
    m_player->setCustomAudioRole(role);
}

void Player::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_slider->setMaximum(m_duration);
}

void Player::positionChanged(qint64 progress)
{
    if (!m_slider->isSliderDown())
        m_slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
        m_player->play();
    }
}

void Player::playlistPositionChanged(int currentItem)
{

    m_playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
}

void Player::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}


void Player::videoAvailableChanged(bool available)
{
    if (!available) {
        disconnect(m_fullScreenButton, &QPushButton::clicked, m_videoWidget, &QVideoWidget::setFullScreen);
        disconnect(m_videoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);
        m_videoWidget->setFullScreen(false);
    } else {
        connect(m_fullScreenButton, &QPushButton::clicked, m_videoWidget, &QVideoWidget::setFullScreen);
        connect(m_videoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);

        if (m_fullScreenButton->isChecked())
            m_videoWidget->setFullScreen(true);
    }
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                          currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
                        m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}


