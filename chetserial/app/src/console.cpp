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

    // 需要先实例化Action,不然会报错。
    setHexModeEnable(true);
    setTimeStampEnable(true);
    setShowSendEnable(true);
    setAutoSendWrapEnable(true);
}

void Console::setHexModeChecked(bool bSet)
{
    m_hexModeSet = bSet;
    hexMenuItem->setChecked(bSet);
}

void Console::setTimeStampChecked(bool bSet)
{
    m_timeStampSet = bSet;
    timeStampMenuItem->setChecked(bSet);
}

void Console::setShowSendChecked(bool bSet)
{
    m_showSend = bSet;
    showSendMenuItem->setChecked(bSet);
}

void Console::setAutoSendWrapChecked(bool bSet)
{
    m_autoSendWrap = bSet;
    autoSendWrapItem->setChecked(bSet);
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
            hexMenuItem = nullptr;
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
            timeStampMenuItem = nullptr;
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
            showSendMenuItem = nullptr;
        }
    }

}

void Console::setAutoSendWrapEnable(bool bSet)
{
    if (bSet)
    {
        autoSendWrapItem = m_menu->addAction(tr("Send Wrap"));
        autoSendWrapItem->setChecked(false);
        autoSendWrapItem->setCheckable(true);
        connect(autoSendWrapItem, &QAction::triggered, this, &Console::setAutoSendWrapChecked);
    }
    else
    {
        if (autoSendWrapItem != nullptr)
        {
            delete autoSendWrapItem;
            autoSendWrapItem = nullptr;
        }
    }
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    m_menu->exec(e->globalPos());
}




