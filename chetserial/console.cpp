#include "console.h"

#include <QScrollBar>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    // 最大行数限制，超过的将从最前面重新开始
    document()->setMaximumBlockCount(100);


//    QPalette p = palette();                 // 获取当前面板属性
//    p.setColor(QPalette::Base, Qt::black);  // 设置输入文本框背景色
//    p.setColor(QPalette::Text, Qt::white);  // 设置输入文本框前景色
//    setPalette(p);                          // 设置面板属性
}


void Console::putData(const QByteArray &data)
{
    // 把数据插入到文本框中
    insertPlainText(data);


    // 垂直滚动条保持在最下方
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnaled = set;
}

//void Console::keyPressEvent(QKeyEvent *e)
//{

//    // 如果设置了本地回显，那么则显示键盘输入的字符
//    if (m_localEchoEnaled)
//    {
//        QPlainTextEdit::keyPressEvent(e);
//    }

//    // 将键盘输入的字符传进信号形参，然后发送可以获取数据的信号
//    emit getData(e->text().toLocal8Bit());

//}


