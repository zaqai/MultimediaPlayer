#include <QMainWindow>
#include <QMap>
#include <QPointer>

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;


class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    void onQuit();
    bool load(const QString &f);
    void sendSlot();//向menu发送信号，将menu显示
signals:
    void mySignal();

public slots:
    void fileNew();

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();


    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);

    void textColor();

    void indent();
    void unindent();


    void clipboardDataChanged();


private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    bool maybeSave();
    void setCurrentFileName(const QString &fileName);
    void modifyIndentation(int amount);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);


    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;

    QAction *actionIndentLess;
    QAction *actionIndentMore;

    QAction *actionUndo;
    QAction *actionRedo;

    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;



    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;
};

