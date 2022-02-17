
#include <QVideoWidget>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent = nullptr);

protected:

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:

    void pressSignal();
};


