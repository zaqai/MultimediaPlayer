#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include"player.h"
#include"imageviewer.h"
#include"textedit.h"
QT_BEGIN_NAMESPACE
namespace Ui { class menu; }
QT_END_NAMESPACE

class menu : public QWidget
{
    Q_OBJECT

public:
    menu(QWidget *parent = nullptr);
    ~menu();
    Player *video;
    ImageViewer *image;
    TextEdit *text;

private slots:
    void on_videoButton_clicked();
    void videoSub();
    void imageSub();
    void textSub();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::menu *ui;

};
#endif // MENU_H
