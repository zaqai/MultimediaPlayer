#include "playercontrols.h"

#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>
#include<QKeyEvent>
PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent)
{

    this->grabKeyboard();       // !!!很重要，添加这个函数，才具有捕获键盘事件的能力！！！
    m_playButton = new QToolButton(this);
    m_playButton->setIcon(QIcon("://icon/play.png"));
    m_playButton->setIconSize(QSize(50,50));
    connect(m_playButton, &QAbstractButton::clicked, this, &PlayerControls::playClicked);

    m_stopButton = new QToolButton(this);
    m_stopButton->setIcon(QIcon("://icon/stop.png"));
    m_stopButton->setIconSize(QSize(50,50));
    m_stopButton->setEnabled(false);

    connect(m_stopButton, &QAbstractButton::clicked, this, &PlayerControls::stop);

    m_nextButton = new QToolButton(this);
    m_nextButton->setIcon(QIcon("://icon/next.png"));
    m_nextButton->setIconSize(QSize(50,50));

    connect(m_nextButton, &QAbstractButton::clicked, this, &PlayerControls::next);

    m_previousButton = new QToolButton(this);
    m_previousButton->setIcon(QIcon("://icon/previous.png"));
    m_previousButton->setIconSize(QSize(50,50));
    connect(m_previousButton, &QAbstractButton::clicked, this, &PlayerControls::previous);

    m_muteButton = new QToolButton(this);
    m_muteButton->setIcon(QIcon("://icon/sound.png"));
    m_muteButton->setIconSize(QSize(50,50));
    connect(m_muteButton, &QAbstractButton::clicked, this, &PlayerControls::muteClicked);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);

    connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayerControls::onVolumeSliderValueChanged);

    m_rateBox = new QComboBox(this);
    m_rateBox->addItem("0.5x", QVariant(0.5));
    m_rateBox->addItem("1.0x", QVariant(1.0));
    m_rateBox->addItem("1.5x", QVariant(1.5));
    m_rateBox->addItem("2.0x", QVariant(2.0));
    m_rateBox->setCurrentIndex(1);

    connect(m_rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayerControls::updateRate);

    QBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_stopButton);
    layout->addWidget(m_previousButton);
    layout->addWidget(m_playButton);
    layout->addWidget(m_nextButton);
    layout->addWidget(m_muteButton);
    layout->addWidget(m_volumeSlider);
    layout->addWidget(m_rateBox);
    setLayout(layout);
}

QMediaPlayer::State PlayerControls::state() const
{
    return m_playerState;
}

void PlayerControls::setState(QMediaPlayer::State state)
{
    if (state != m_playerState) {
        m_playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            m_stopButton->setEnabled(false);
            m_playButton->setIcon(QIcon("://icon/play.png"));
            break;
        case QMediaPlayer::PlayingState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(QIcon("://icon/pause.png"));
            m_playButton->setIconSize(QSize(50,50));
            break;
        case QMediaPlayer::PausedState:
            m_stopButton->setEnabled(true);
            m_playButton->setIcon(QIcon("://icon/play.png"));
            break;
        }
    }
}

int PlayerControls::volume() const
{
    qreal linearVolume =  QAudio::convertVolume(m_volumeSlider->value() / 100.0,QAudio::LogarithmicVolumeScale,QAudio::LinearVolumeScale);
    //linearVolume返回0~1，qreal（double）
    return qRound(linearVolume * 100);//qRound取最近整数
}

void PlayerControls::setVolume(int volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume /100.0,
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

bool PlayerControls::isMuted() const
{
    return m_playerMuted;
}

void PlayerControls::setMuted(bool muted)
{
    if (muted != m_playerMuted) {
        m_playerMuted = muted;

        m_muteButton->setIcon(muted ? QIcon("://icon/sound_muted.png"):QIcon("://icon/sound.png"));
    }}

void PlayerControls::playClicked()
{
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
}

void PlayerControls::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}

qreal PlayerControls::playbackRate() const
{
    return m_rateBox->itemData(m_rateBox->currentIndex()).toDouble();
    //currentIndex : int    此特性保持组合框当前项的索引。
    //itemData     Returns the data for the given role in the given index in the combobox
}

void PlayerControls::setPlaybackRate(float rate)
{
    for (int i = 0; i < m_rateBox->count(); ++i) {
        if (qFuzzyCompare(rate, float(m_rateBox->itemData(i).toDouble()))) {
            //找到设定速率的索引
            m_rateBox->setCurrentIndex(i);
            return;
        }
    }

}

void PlayerControls::updateRate()
{
    emit changeRate(playbackRate());
}

void PlayerControls::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}


void PlayerControls::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Space :
        playClicked();
        break;
    case Qt::Key_End :
        PlayerControls::next();
        break;
    case Qt::Key_Home :
        PlayerControls::previous();
        break;
    case Qt::Key_PageUp :
        emit changeVolume(volume()+10);
        break;
    case Qt::Key_PageDown :
        emit changeVolume(volume()-10);
        break;
    }
}
