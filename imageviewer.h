#include <QMainWindow>
#include <QImage>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = nullptr);
    bool loadFile(const QString &);
    void sendSlot();//向menu发送信号，将menu显示
private slots:
    void open();
    void save();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    void onQuit();

signals:
    void mySignal();

private:
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(double factor);

    void wheelEvent(QWheelEvent *event);

    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor = 1;

    QAction *saveAct;

    QAction *copyAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
};


