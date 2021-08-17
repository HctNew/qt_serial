#ifndef XMLHELPER_H
#define XMLHELPER_H

#include <QObject>

QT_BEGIN_NAMESPACE

class QString;
class QDomDocument;

QT_END_NAMESPACE

/**
 * @brief create XML file
 * @param filePath  file Dir
 * @param fileName  file name
 * @return bool
 */
bool createXml(QString filePath, QString fileName);

/**
 * @brief read xml
 * @param [in]filePath      file relative path
 * @param [out]doc          xml data
 * @return
 */
bool xmlRead(const QString &filePath, QDomDocument &doc);

/**
 * @brief write xml
 * @param [in]filePath  file relative path
 * @param [in]doc       xml data
 * @return
 */
bool xmlWrite(const QString &filePath, const QDomDocument &doc);

#endif // XMLHELPER_H
