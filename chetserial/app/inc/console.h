#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>

QT_BEGIN_NAMESPACE

class QAction;

QT_END_NAMESPACE

class Console : public QTextEdit
{
    Q_OBJECT


signals:
    void getData(const QByteArray &data);

private slots:
    void setHexModeChecked(bool   bSet){ m_hexModeSet   = bSet;   };
    void setTimeStampChecked(bool bSet){ m_timeStampSet = bSet;   };
    void setShowSendChecked(bool  bSet){ m_showSend     = bSet;   };


public:
    explicit Console(QWidget *parent = nullptr);

    void showData(const QString &data);
    void setLocalEchoEnabled(bool bSet) { m_localEchoEnaled = bSet; };

public:
    void setHexModeEnable(bool   bSet);
    void setTimeStampEnable(bool bSet);
    void setShowSendEnable(bool  bSet);
    bool isHexModeChecked(void)   { return m_hexModeSet;   };
    bool isTimeStampChecked(void) { return m_timeStampSet; };
    bool isShowSendChecked(void)  { return m_showSend;     };

protected:
    void contextMenuEvent(QContextMenuEvent *e) override; // 右键菜单

private:
    QMenu   *m_menu             = nullptr;
    QAction *hexMenuItem        = nullptr;
    QAction *timeStampMenuItem  = nullptr;
    QAction *showSendMenuItem   = nullptr;
    bool m_hexModeSet           = false;
    bool m_showSend             = false;
    bool m_timeStampSet         = false;
    bool m_localEchoEnaled      = false;
};

#endif // CONSOLE_H
