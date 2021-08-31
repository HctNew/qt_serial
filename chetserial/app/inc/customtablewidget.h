#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <QDomElement>

#define ITEMROW_DEFAULTHEIGHT   (22)    ///< 行默认高度

QT_BEGIN_NAMESPACE

class QTableWidget;
class QDomDocument;

namespace Ui { class CustomTableWidget; }

QT_END_NAMESPACE

class CustomTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    enum HorizontalTableHeaderColumn
    {
        HEX_COLUMN = 0,
        WRAP_COLUMN,
        CMD_COLUMN,
        DESC_COLUMN,
        CONT_COLUMN,
    };

    typedef struct CmdStruct
    {
        bool m_isHexChecked  = false;
        bool m_isWrapChecked = false;
        int  m_id            = 0;
        int  m_rowHeight     = ITEMROW_DEFAULTHEIGHT;
        QString m_desc       = QStringLiteral("");
        QString m_content    = QStringLiteral("");
    }CmdStruct;

public:
    CustomTableWidget(QWidget *parent=nullptr);

    void addItemRow(const CmdStruct & cmdData);

    void removeItemRow( int  itemRow);
    void moveItemRow(   int  fromRow, int toRow);
    bool isHexChecked(  int  itemRow);
    bool isWrapChecked( int  itemRow);
    void setEditEnable( bool isEdited);

    bool xmlInitCmdList(const QString &xmlFile);
    bool xmlSaveCmdList(const QString &xmlFile);
    bool xmlLoadCmdList(const QString &xmlFile);

private:

    void addCmdToXml(QDomDocument doc, QDomElement parentElem, const CmdStruct & cmdData);
    void removeCmdFromXml(QDomElement parentElem, int id)
    {
        parentElem.removeChild(parentElem.childNodes().at(id));
    }
    bool changeCmdFromXml(QDomElement parentElem, const CmdStruct & cmdData);
    bool getCmdFromXml(QDomElement parentElem, CmdStruct &cmdData);
    int  getCmdCountFromXml(QDomElement parentElem)
    {
        return parentElem.childNodes().length();
    }

signals:
    void commandClicked(bool isHexChecked, bool isWrapChecked, const QString &data);

 private slots:
    void itemChangedSlot(QTableWidgetItem *selectItem);
    void itemButtonClicked(void);


};

#endif // CUSTOMTABLEWIDGET_H
