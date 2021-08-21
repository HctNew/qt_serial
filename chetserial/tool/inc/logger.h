#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

QT_BEGIN_NAMESPACE


QT_END_NAMESPACE


namespace logger {

    void write(const QString &filePath, const QString &msg);

}

#endif // LOGGER_H
