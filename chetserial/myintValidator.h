#ifndef MYINTVALIDATOR_H
#define MYINTVALIDATOR_H

#include <QIntValidator>

class MyValidator : public QIntValidator
{
    Q_OBJECT
public:
    explicit MyValidator(QObject * parent = 0):QIntValidator(parent)
    {
    }

    MyValidator(int bottom, int top, QObject * parent):QIntValidator(bottom, top, parent)
    {
    }

    void setRange(int bottom, int top)
    {
        QIntValidator::setRange(bottom, top);
    }

    ~MyValidator()
    {
    }

    State validate(QString &s, int &n) const
    {
        return QIntValidator::validate(s, n);
    }

    void fixup(QString &s) const
    {
        s = QString("%1").arg(top());
    }
};

#endif // MYINTVALIDATOR_H
