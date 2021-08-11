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
    void setHexModeEnable(bool bSet) { m_hexModeSet = bSet; };

public:
    explicit Console(QWidget *parent = nullptr);

    void readData(const QByteArray &data);
    void setLocalEchoEnabled(bool set);
    bool isHexMode(void) { return m_hexModeSet; };

private:
    void initContextMenu();

protected:
    void contextMenuEvent(QContextMenuEvent *e) override; // 右键菜单

private:
    bool m_hexModeSet = false;
    bool m_localEchoEnaled = false;
    bool m_isHexSend = false;
    bool m_isHexRecv = false;
    QMenu *m_menu = nullptr;

};

#endif // CONSOLE_H
