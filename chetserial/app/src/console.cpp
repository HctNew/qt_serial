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
    //document()->setMaximumBlockCount(100);

    //默认的标准右键菜单，如果不需要刻意完全自己实现
    m_menu = createStandardContextMenu();
    setHexModeEnable(true);
    setTimeStampEnable(true);
    setShowSendEnable(true);
}


void Console::showData(const QString &data)
{
    // 把数据插入到文本框中
    insertPlainText(data);


    // 垂直滚动条保持在最下方
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setHexModeEnable(bool bSet)
{
    if (bSet)
    {
        hexMenuItem = m_menu->addAction(tr("HEX Mode"));
        hexMenuItem->setChecked(false);
        hexMenuItem->setCheckable(true);
        connect(hexMenuItem, &QAction::triggered, this, &Console::setHexModeChecked);
    }
    else
    {
        if (hexMenuItem != nullptr)
        {
            delete hexMenuItem;
        }
    }
}

void Console::setTimeStampEnable(bool bSet)
{
    if (bSet)
    {
        timeStampMenuItem = m_menu->addAction(tr("Time Stamp"));
        timeStampMenuItem->setChecked(false);
        timeStampMenuItem->setCheckable(true);
        connect(timeStampMenuItem, &QAction::triggered, this, &Console::setTimeStampChecked);
    }
    else
    {
        if (timeStampMenuItem != nullptr)
        {
            delete timeStampMenuItem;
        }
    }
}

void Console::setShowSendEnable(bool bSet)
{
    if (bSet)
    {
        showSendMenuItem = m_menu->addAction(tr("Show Send"));
        showSendMenuItem->setChecked(false);
        showSendMenuItem->setCheckable(true);
        connect(showSendMenuItem, &QAction::triggered, this, &Console::setShowSendChecked);
    }
    else
    {
        if (showSendMenuItem != nullptr)
        {
            delete showSendMenuItem;
        }
    }

}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    m_menu->exec(e->globalPos());
}




