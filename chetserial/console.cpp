#include "console.h"

#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMessageBox>


Console::Console(QWidget *parent) :
    QTextEdit(parent)
{
    // 最大行数限制，超过的将从最前面重新开始
    document()->setMaximumBlockCount(100);

    initContextMenu();
}



void Console::readData(const QByteArray &data)
{
    if (data.isEmpty()) return;

    QString strDis;

    // 使用十六进制显示
    if (m_hexModeSet == true)
    {

        QString strHex = data.toHex().data();   // 转化为HEX字符串
        strHex = strHex.toUpper();              // HEX字符串大写

        // HEX之间使用空格隔开
        for(int i=0; i<strHex.length(); i+=2)
        {
            QString st = strHex.mid(i,2);
            strDis += st;
            strDis += " ";
        }

//        strDis = QString(data.toHex(' ').toUpper().append(' '));

    }
    else
    {
        strDis = tr(data);
    }

    // 把数据插入到文本框中
    insertPlainText(strDis);


    // 垂直滚动条保持在最下方
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}


void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnaled = set;
}

void Console::initContextMenu()
{
    //默认的标准右键菜单，如果不需要刻意完全自己实现
    m_menu = createStandardContextMenu();

    QAction *hexMenuItem = m_menu->addAction(tr("HEX Mode"));
    hexMenuItem->setChecked(false);
    hexMenuItem->setCheckable(true);

    connect(hexMenuItem, &QAction::triggered, this, &Console::setHexModeEnable);
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    m_menu->exec(e->globalPos());
}




