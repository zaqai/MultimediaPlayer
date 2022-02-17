#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QMimeDatabase>


#include "textedit.h"

TextEdit::TextEdit(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(800,600);
    setWindowTitle("textEditor");
    textEdit = new QTextEdit(this);

    setCentralWidget(textEdit);


    setupFileActions();
    setupEditActions();
    setupTextActions();
    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());


    connect(textEdit->document(), &QTextDocument::modificationChanged,
            actionSave, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::modificationChanged,
            this, &QWidget::setWindowModified);
    connect(textEdit->document(), &QTextDocument::undoAvailable,
            actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable,
            actionRedo, &QAction::setEnabled);

    setWindowModified(textEdit->document()->isModified());
    actionSave->setEnabled(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());


    actionCut->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);
    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);

    textEdit->setFocus();
    setCurrentFileName(QString());


}

void TextEdit::closeEvent(QCloseEvent *e)
{
    if (maybeSave())
        e->accept();
    else
        e->ignore();
}

void TextEdit::setupFileActions()      //建立file菜单项
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));

    QIcon newIcon = QIcon("://icon/filenew.png");
    QAction *a = menu->addAction(newIcon,  tr("New"), this, &TextEdit::fileNew);
    tb->addAction(a);

    a->setShortcut(QKeySequence::New);

    const QIcon openIcon = QIcon("://icon/fileopen.png");
    a = menu->addAction(openIcon, tr("Open"), this, &TextEdit::fileOpen);
    a->setShortcut(QKeySequence::Open);
    tb->addAction(a);

    menu->addSeparator();

    const QIcon saveIcon = QIcon("://icon/filesave.png");
    actionSave = menu->addAction(saveIcon, tr("&Save"), this, &TextEdit::fileSave);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    tb->addAction(actionSave);

    a = menu->addAction(tr("Save As..."), this, &TextEdit::fileSaveAs);

    menu->addSeparator();



    a = menu->addAction(tr("Quit"), this, &TextEdit::onQuit);
    a->setShortcut(tr("ctrl+Q"));
}

void TextEdit::setupEditActions()   //建立edit菜单项
{
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon("://icon/editundo.png");
    actionUndo = menu->addAction(undoIcon, tr("Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon("://icon/editredo.png");
    actionRedo = menu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);

    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();

    const QIcon cutIcon = QIcon("://icon/editcut.png");
    actionCut = menu->addAction(cutIcon, tr("Cut"), textEdit, &QTextEdit::cut);

    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon("://icon/editcopy.png");
    actionCopy = menu->addAction(copyIcon, tr("Copy"), textEdit, &QTextEdit::copy);

    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon("://icon/editpaste.png");
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);

    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        //检测一下当前剪切板内容是否为文字
        actionPaste->setEnabled(md->hasText());

}

void TextEdit::setupTextActions()          //建立format菜单项
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("Format"));

    const QIcon boldIcon = QIcon("://icon/textbold.png");
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);

    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);  //设置bold菜单项显示效果为黑体
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon("://icon/textitalic.png");
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);

    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);//设置italic菜单项显示效果为斜体
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon("://icon/textunder.png");
    actionTextUnderline = menu->addAction(underlineIcon, tr("Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);

    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);//设置Underline菜单项显示效果为下划线
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();





    const QIcon indentMoreIcon = QIcon("://icon/format-indent-more.png");
    actionIndentMore = menu->addAction(indentMoreIcon, tr("&Indent"), this, &TextEdit::indent);
    actionIndentMore->setShortcut(Qt::CTRL + Qt::Key_BracketRight);

    const QIcon indentLessIcon = QIcon("://icon/format-indent-less.png");
    actionIndentLess = menu->addAction(indentLessIcon, tr("&Unindent"), this, &TextEdit::unindent);
    actionIndentLess->setShortcut(Qt::CTRL + Qt::Key_BracketLeft);




    tb->addAction(actionIndentMore);
    tb->addAction(actionIndentLess);
    menu->addAction(actionIndentMore);
    menu->addAction(actionIndentLess);

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);//画图作为color菜单项icon，16*16，填充黑色
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    menu->addSeparator();



    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);


    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, &QComboBox::textActivated, this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, &QComboBox::textActivated, this, &TextEdit::textSize);
}

bool TextEdit::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) {
        QUrl baseUrl = (f.front() == QLatin1Char(':') ? QUrl(f) : QUrl::fromLocalFile(f)).adjusted(QUrl::RemoveFilename);
        textEdit->document()->setBaseUrl(baseUrl);
        textEdit->setHtml(str);
    } else {
#if QT_CONFIG(textmarkdownreader)
        QMimeDatabase db;
        if (db.mimeTypeForFileNameAndData(f, data).name() == QLatin1String("text/markdown"))
            textEdit->setMarkdown(QString::fromUtf8(data));
        else
#endif
            textEdit->setPlainText(QString::fromUtf8(data));
    }

    setCurrentFileName(f);
    return true;
}

bool TextEdit::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret =
            QMessageBox::warning(this, QCoreApplication::applicationName(),
                                 tr("The document has been modified.\n"
                                    "Do you want to save your changes?"),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void TextEdit::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(shownName);
    setWindowModified(false);
}

void TextEdit::fileNew()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFileName(QString());
    }
}

void TextEdit::fileOpen()
{
    QFileDialog fileDialog(this, tr("Open File..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    fileDialog.setMimeTypeFilters(QStringList()

                                  << "text/html"

                                  << "text/markdown"

                                  << "text/plain");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().first();
    if (load(fn))
        statusBar()->showMessage(tr("Opened \"%1\"").arg(QDir::toNativeSeparators(fn)));
    else
        statusBar()->showMessage(tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(fn)));
}

bool TextEdit::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(textEdit->document());
    if (success) {
        textEdit->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    } else {
        statusBar()->showMessage(tr("Could not write to file \"%1\"")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
    return success;
}

bool TextEdit::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "text/plain"

              << "application/vnd.oasis.opendocument.text"

              << "text/markdown"

              << "text/html";
    fileDialog.setMimeTypeFilters(mimeTypes);

    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}




void TextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}


void TextEdit::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);//字体颜色
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);//改变一下color的icon颜色
}


void TextEdit::indent()
{
    modifyIndentation(1);
}

void TextEdit::unindent()
{
    modifyIndentation(-1);
}

void TextEdit::modifyIndentation(int amount)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();
    blockFmt.setIndent(blockFmt.indent() + amount);
    cursor.setBlockFormat(blockFmt);
    //    }
    cursor.endEditBlock();
}



void TextEdit::clipboardDataChanged()//剪切板内容有变
{

    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());//如果是文本就让paste按钮可用

}


void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    //选择光标下的单词。如果光标未定位在可选择字符的字符串中，则不会选择文本。
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}


void TextEdit::sendSlot()
{
    emit mySignal();
}


void TextEdit::onQuit()
{
    emit mySignal();
    QWidget::close();
}
