
#include "xmlhelper.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>



/**
 * @brief create XML file
 * @param filePath  file Dir
 * @param fileName  file name
 * @return bool
 */
bool createXml(QString filePath, QString fileName)
{
    QDir tempDir;
    QFile tempFile;

    // 如果文件不存在则创建，由于QT自带QFile不支持递归创建，所以得手动配置路径，先创建文件夹再创建文件。
    tempFile.setFileName(fileName);

    //临时保存程序当前路径
    QString currentDir = tempDir.currentPath();

    // 如果filePath路径不存在，创建它
    if(!tempDir.exists(filePath))
    {
        tempDir.mkpath(filePath);
    }

    // 将程序的执行路径设置到filePath下
    tempDir.setCurrent(filePath);

    // WriteOnly方式：以只写方式打开文件，没有则创建
    tempFile.open(QIODevice::WriteOnly);

    QDomDocument doc;
    QDomElement root = doc.documentElement();

    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    root = doc.createElement(QString("root"));
    doc.appendChild(root); //添加根元素

    QTextStream out_stream(&tempFile);
    doc.save(out_stream, 4); //缩进4格

    tempFile.close();

    //将程序当前路径设置为原来的路径
    tempDir.setCurrent(currentDir);

    return true;
}

/**
 * @brief read xml
 * @param [in]filePath      file relative path
 * @param [out]doc          xml data
 * @return
 */
bool xmlRead(const QString &filePath, QDomDocument &doc)
{
    QFile file(filePath);
    QDir  dir;

    // 判断文件能否打开
    if (!file.open(QIODevice::ReadOnly))
    {
//        QMessageBox::critical(nullptr, QObject::tr("Error"),
//                              QObject::tr("%1/%2 open failed.").arg(dir.currentPath(), filePath));
        return false;
    }

   // setContent是将指定的内容指定给QDomDocument解析，***参数可以是QByteArray或者是文件名等
   if(!doc.setContent(&file))
   {
       file.close();
       return false;
   }

   file.close();
   return true;
}

/**
 * @brief write xml
 * @param [in]filePath  file relative path
 * @param [in]doc       xml data
 * @return
 */
bool xmlWrite(const QString &filePath, const QDomDocument &doc)
{
    QFile file(filePath);
    QDir  dir;

    // 判断文件能否打开
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
//        QMessageBox::critical(nullptr, QObject::tr("Error"),
//                              QObject::tr("%1/%2 open failed.").arg(dir.currentPath(), filePath));
        return false;
    }

    QTextStream out_stream(&file);
    doc.save(out_stream, 4); //缩进4格

    file.close();
    return true;
}


