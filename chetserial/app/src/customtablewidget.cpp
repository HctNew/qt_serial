#include "customtablewidget.h"
#include "xmlhelper.h"

#include <QObject>
#include <QTableWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyleFactory>
#include <QHeaderView>
#include <QSizePolicy>
#include <QWidget>
#include <QToolTip>
#include <QDomDocument>

#define XML_NODE_CMDLIST    ("cmdlist")

CustomTableWidget::CustomTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    // 设置边框以及颜色
    setStyleSheet("CustomTableWidget{border:2px solid gray;}");

    setSelectionBehavior(QAbstractItemView::SelectRows);    // 以行为单位
    setSelectionMode(QAbstractItemView::SingleSelection);   // 选中模式为单行选中


    horizontalHeader()->setStretchLastSection(true);        // 行头自适应表格
    horizontalHeader()->setHighlightSections(false);        // 点击表时不对表头行光亮（获取焦点）
    horizontalHeader()->setStyleSheet("QHeaderView::section{border: 1px solid lightgray; "
                                      "background: #aa557f; "
                                      "font: Bold 9pt consolas;}"); // 设置边框样式
    verticalHeader()->setStyleSheet("QHeaderView::section{border: 1px solid lightgray;"
                                    "font: Bold 9pt consolas;}");
    verticalHeader()->setHighlightSections(false);          // 点击表时不对表头行光亮（获取焦点）
    verticalHeader()->setDefaultAlignment(Qt::AlignCenter); // 设置垂直列表头的对齐为居中
    verticalHeader()->setMinimumWidth(25);                  // 设置垂直列表头的宽度，即行号所在的列
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setEditTriggers(QAbstractItemView::DoubleClicked);  // 设置为双击输入，防止单击输入导致系统崩溃

    // 添加行表头
    QStringList headerText;
    headerText << tr("HEX") << tr("WRAP") << tr("CMD") << tr("Desc") << tr("Content");

    setColumnCount(headerText.count());
    setColumnWidth(HEX_COLUMN,  30);
    setColumnWidth(WRAP_COLUMN, 31);
    setColumnWidth(CMD_COLUMN,  60);
    setColumnWidth(DESC_COLUMN, 80);
    setColumnWidth(CONT_COLUMN, 100);
    setHorizontalHeaderLabels(headerText);

    connect(this, &QTableWidget::itemChanged, this, &CustomTableWidget::itemChangedSlot);
}

void CustomTableWidget::addItemRow(CmdStruct & cmdData)
{
    insertRow(cmdData.m_id);
    setRowHeight(cmdData.m_id, cmdData.m_rowHeight);

    {
        QWidget     *widget  = new QWidget(this);
        QCheckBox   *chkBox  = new QCheckBox(widget);
        QHBoxLayout *hLayout = new QHBoxLayout(widget); //实例化水平布局

        chkBox->setChecked(cmdData.m_isHexChecked);
        hLayout->addWidget(chkBox); //  添加checkbox
        hLayout->setMargin(0);      // 设置布局里控件离边缘的间隙
        hLayout->setAlignment(chkBox, Qt::AlignCenter); //居中
        widget->setLayout(hLayout);

        setCellWidget(cmdData.m_id, HEX_COLUMN, widget);
        connect(chkBox, SIGNAL(clicked()), this, SLOT(itemCheckClicked()));
    }

    {
        QWidget     *widget  = new QWidget(this);
        QCheckBox   *chkBox  = new QCheckBox(widget);
        QHBoxLayout *hLayout = new QHBoxLayout(widget); //实例化水平布局



        if (cmdData.m_isHexChecked)
        {
            cmdData.m_isWrapChecked = false;
            chkBox->setEnabled(false);
        }
        chkBox->setChecked(cmdData.m_isWrapChecked);
        hLayout->addWidget(chkBox); //  添加checkbox
        hLayout->setMargin(0);      // 设置布局里控件离边缘的间隙
        hLayout->setAlignment(chkBox, Qt::AlignCenter); //居中
        widget->setLayout(hLayout);

        setCellWidget(cmdData.m_id, WRAP_COLUMN, widget);
    }


    {
        QWidget     *pWidget  = new QWidget(this);
        QPushButton *pushBtn  = new QPushButton(cmdData.m_desc, pWidget);
        QHBoxLayout *phLayout = new QHBoxLayout(pWidget); //实例化水平布局

        pushBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        phLayout->addWidget(pushBtn);
        phLayout->setMargin(0);
        pWidget->setLayout(phLayout);

        connect(pushBtn, SIGNAL(clicked()), this, SLOT(itemButtonClicked()));
        setCellWidget(cmdData.m_id, CMD_COLUMN, pWidget);
    }



    setItem(cmdData.m_id, DESC_COLUMN, new QTableWidgetItem(cmdData.m_desc));
    setItem(cmdData.m_id, CONT_COLUMN, new QTableWidgetItem(cmdData.m_content));
    item(cmdData.m_id, CONT_COLUMN)->setToolTip("Hex format:[xx xx xx]");

    if (cmdData.m_id >= 0)
    {
        selectRow(cmdData.m_id);
    }
}

void CustomTableWidget::removeItemRow(int itemRow)
{
    if (itemRow < 0) return;

    if (item(itemRow, DESC_COLUMN) == nullptr) return;

    removeRow(itemRow);

    if (itemRow-1 >= 0)
    {
        selectRow(itemRow-1);
    }

}

void CustomTableWidget::moveItemRow(int fromRow, int toRow)
{
    int nRowCount    = rowCount();
    int nColumnCount = columnCount();

    //setFocus();

    if (fromRow == toRow) return;

    if (fromRow < 0          || toRow < 0      ||
        fromRow >= nRowCount || toRow >= nRowCount) return;

    int fromRowHeight = rowHeight(fromRow);
    /* 向上移动:
     * a. 应在toRow处插入新行，toRow值不变.
     * b. fromRow所在目标下移，则fromRow需要加1.
     *
     * 向下移动:
     * a. 应在toRow的下方插入新行，toRow值需要加1.
     * b. fromRow所在目标不变，则fromRow值不变.
     *
    */
    if (toRow   < fromRow) fromRow++;
    if (fromRow < toRow)   toRow++;

    insertRow(toRow);

    for (int i=0; i < nColumnCount; i++)
    {
        // 将fromRow行的每一列复制到toRow,并使用takeItem删除fromRow处的item,
        // 第0列处的是cellWidget，有CheckBox控件，需要额外处理
        if (i <= CMD_COLUMN)
        {
            // 将fromRow处的widget移动到toRow处，这里不能用removeCellWidget删除，不然fromRow处的cellWidget就被析构了
            setCellWidget(toRow, i, cellWidget(fromRow, i));
            setCellWidget(toRow, i, cellWidget(fromRow, i));
            takeItem(fromRow, i);
        }
        else
        {
            setItem(toRow, i , takeItem(fromRow, i));
        }

    }


    if (fromRow < toRow) toRow--;

    removeRow(fromRow);
    setRowHeight(toRow, fromRowHeight);
    selectRow(toRow);
}


bool CustomTableWidget::isHexChecked(int itemRow)
{
    QWidget *pWidget = cellWidget(itemRow, HEX_COLUMN);

    // 找出指定行列处的widget下的所有QCheckBox
    QList<QCheckBox *> allCheckBoxs =  pWidget->findChildren<QCheckBox *>();

    if(allCheckBoxs.size() > 0)
    {
        return allCheckBoxs.first()->isChecked();
    }

    return false;
}

bool CustomTableWidget::isWrapChecked(int itemRow)
{
    QWidget *pWidget = cellWidget(itemRow, WRAP_COLUMN);

    // 找出指定行列处的widget下的所有QCheckBox
    QList<QCheckBox *> allCheckBoxs =  pWidget->findChildren<QCheckBox *>();

    if(allCheckBoxs.size() > 0)
    {
        return allCheckBoxs.first()->isChecked();
    }

    return false;
}

void CustomTableWidget::setEditEnable(bool isEdited)
{
    showColumn(HEX_COLUMN);
    showColumn(WRAP_COLUMN);
    if (isEdited)
    {
        hideColumn(CMD_COLUMN);
        showColumn(DESC_COLUMN);
        showColumn(CONT_COLUMN);
        setFixedWidth(320);
    }
    else
    {
        showColumn(CMD_COLUMN);
        hideColumn(DESC_COLUMN);
        hideColumn(CONT_COLUMN);
        setFixedWidth(170);
    }
}

bool CustomTableWidget::xmlInitCmdList(const QString &xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_CMDLIST));
    QDomElement  parentElem = nodeList.at(0).toElement();

    if (!parentElem.isNull()) return true;

    parentElem = doc.createElement(QString(XML_NODE_CMDLIST));
    root.appendChild(parentElem);

    CmdStruct cmdData;
    cmdData.m_id = 0;
    addCmdToXml(doc, parentElem, cmdData);

    return xmlHelper::xmlWrite(xmlFile, doc);
}

bool CustomTableWidget::xmlSaveCmdList(const QString &xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_CMDLIST));
    QDomElement  parentElem = nodeList.at(0).toElement();   // options元素

    int xmlCmdCount = getCmdCountFromXml(parentElem);
    int tableRowCount = rowCount();
    if (tableRowCount < xmlCmdCount)
    {
        // 如果xml的命令比tablewidget的命令行数要多，则删掉xml多余的命令。
        for (int ii=xmlCmdCount-1; ii>=tableRowCount; ii--)
        {
            removeCmdFromXml(parentElem, ii);
        }
    }

    for (int ii=0; ii<tableRowCount; ii++)
    {
        CmdStruct cmdData;
        cmdData.m_id = ii;
        cmdData.m_isHexChecked  = isHexChecked(ii);
        cmdData.m_isWrapChecked = isWrapChecked(ii);
        cmdData.m_rowHeight     = rowHeight(ii);
        cmdData.m_desc          = item(ii, DESC_COLUMN)->text();
        cmdData.m_content       = item(ii, CONT_COLUMN)->text();

        // 修改xml的cmd内容
        if (changeCmdFromXml(parentElem, cmdData) == false)
        {
            // 如果没有此项，则添加
            addCmdToXml(doc, parentElem, cmdData);
        }
    }

    return xmlHelper::xmlWrite(xmlFile, doc);
}

bool CustomTableWidget::xmlLoadCmdList(const QString &xmlFile)
{
    QDomDocument doc;

    if (false == xmlHelper::xmlRead(xmlFile, doc)) return false;

    QDomElement  root       = doc.documentElement();      // 返回根节点
    QDomNodeList nodeList   = root.elementsByTagName(QString(XML_NODE_CMDLIST));
    QDomElement  parentElem = nodeList.at(0).toElement();


    int xmlCmdCount = getCmdCountFromXml(parentElem);

    nodeList = parentElem.childNodes();
    for (int ii=0; ii<xmlCmdCount; ii++)
    {
        CmdStruct cmdData;
        cmdData.m_id = ii;

        if (getCmdFromXml(parentElem, cmdData) == true)
        {
            addItemRow(cmdData);
        }
    }

    return true;
}

void CustomTableWidget::addCmdToXml(QDomDocument doc, QDomElement parentElem,
                                    const CustomTableWidget::CmdStruct & cmdData)
{
    QDomElement childelem = doc.createElement(QStringLiteral("CMD"));
    childelem.setAttribute(QStringLiteral("id"), cmdData.m_id);
    parentElem.appendChild(childelem);

    QStringList childNodeStr;
    childNodeStr << QStringLiteral("HEX")       << QString::number(cmdData.m_isHexChecked)    <<
                    QStringLiteral("WRAP")      << QString::number(cmdData.m_isWrapChecked)   <<
                    QStringLiteral("RowHeight") << QString::number(cmdData.m_rowHeight)       <<
                    QStringLiteral("DESC")      << cmdData.m_desc                             <<
                    QStringLiteral("CONT")      << cmdData.m_content;


    for (qint8 ii=0; ii<6; ii+=2)
    {
        QDomElement elem = doc.createElement(childNodeStr.at(ii));
        QDomText    text = doc.createTextNode(childNodeStr.at(ii+1));
        elem.appendChild(text);
        childelem.appendChild(elem);
    }

    for (qint8 ii=6; ii<10; ii+=2)
    {
        QDomElement elem = doc.createElement(childNodeStr.at(ii));
        elem.setAttribute("str", childNodeStr.at(ii+1));
        childelem.appendChild(elem);
    }
}

bool CustomTableWidget::changeCmdFromXml(QDomElement parentElem,
                                         const CustomTableWidget::CmdStruct &cmdData)
{
    QDomNodeList nodeList = parentElem.childNodes();    // cmdlist的子节点
    QDomElement  elem = nodeList.at(cmdData.m_id).toElement();

    if (elem.isNull()) return false;

    nodeList = elem.childNodes();  // 指定id即CMD的子节点

    elem.setAttribute(QStringLiteral("id"), cmdData.m_id);
    nodeList.at(0).firstChild().setNodeValue(QString::number(cmdData.m_isHexChecked));
    nodeList.at(1).firstChild().setNodeValue(QString::number(cmdData.m_isWrapChecked));
    nodeList.at(2).firstChild().setNodeValue(QString::number(cmdData.m_rowHeight));
    nodeList.at(3).toElement().setAttribute("str", cmdData.m_desc);
    nodeList.at(4).toElement().setAttribute("str", cmdData.m_content);

    return true;
}

bool CustomTableWidget::getCmdFromXml(QDomElement parentElem, CustomTableWidget::CmdStruct &cmdData)
{
    QDomNodeList nodeList = parentElem.childNodes();    // cmdlist的子节点
    QDomElement  elem = nodeList.at(cmdData.m_id).toElement();

    if (elem.isNull()) return false;

    nodeList = elem.childNodes();  // 指定id即CMD的子节点
    cmdData.m_id = elem.attribute(QStringLiteral("id")).toInt();

    cmdData.m_isHexChecked  = nodeList.at(0).firstChild().nodeValue().toInt();
    cmdData.m_isWrapChecked = nodeList.at(1).firstChild().nodeValue().toInt();
    cmdData.m_rowHeight     = nodeList.at(2).firstChild().nodeValue().toInt();
    cmdData.m_desc          = nodeList.at(3).toElement().attribute("str");
    cmdData.m_content       = nodeList.at(4).toElement().attribute("str");

    return true;
}


void CustomTableWidget::itemChangedSlot(QTableWidgetItem *selectItem)
{

    // 如果desc栏目下的text改变，则先将button栏的cellWidget删除，
    // 然后使用desc栏目下的text构造一个新的button.
    if (selectItem->column() != DESC_COLUMN) return;

    QWidget *pWidget = cellWidget(selectItem->row(), CMD_COLUMN);

    // 找出指定行列处的widget下的所有QPushButton
    QList<QPushButton *> allPushBtns =  pWidget->findChildren<QPushButton *>();

    if(allPushBtns.size() == 0) return;

    // 将Desc栏下的内容作为button的标题
    QPushButton *pushBtn = allPushBtns.first();
    QString     itemText = item(selectItem->row(), selectItem->column())->text();

    pushBtn->setText(itemText);
}

void CustomTableWidget::itemCheckClicked(void)
{
    QCheckBox   *chkBox     = qobject_cast<QCheckBox*>(sender());
    QWidget     *w_parent   = (QWidget*)chkBox->parent();

    QModelIndex index = indexAt(w_parent->mapToParent(chkBox->pos()));
    int row = index.row();

    // 找出Wrap checkbox
    QWidget *pWidget = cellWidget(row, WRAP_COLUMN);
    QList<QCheckBox *> allCheckBoxs =  pWidget->findChildren<QCheckBox *>();

    allCheckBoxs.first()->setChecked(false);
    if(allCheckBoxs.size() > 0)
    {
        allCheckBoxs.first()->setEnabled(!chkBox->isChecked());
    }

}

void CustomTableWidget::itemButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    QWidget *w_parent = (QWidget*)btn->parent();

    /* 前面添加pushbutton时已经指定了button的parent就是setCellWidget的Widget，可以查看方法 addItemRow
     *
     * w_parent->mapToParent(btn->pos()): 将w_parent下坐标btn的坐标转换为w_parent的parent(this)的坐标
     * indexAt: 使用tableView的方法indexAt, 返回关于这个pos位置处的一些信息，比如行列。
     */
    QModelIndex index = indexAt(w_parent->mapToParent(btn->pos()));

    int row = index.row();

    // 如果是没有被编辑过的单元格，将不能用ui->tableWidget->item(i,j)->text()访问，因为该指针ui->tableWidget->item(i,j)为空。
    if (item(row, CONT_COLUMN)==nullptr || item(row, CONT_COLUMN)->text().isEmpty()) return;

    emit commandClicked(isHexChecked(row), isWrapChecked(row), item(row, CONT_COLUMN)->text());

}

