#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE

class QFont;
class QColor;

namespace Ui {
class OptionsDialog;
}

QT_END_NAMESPACE

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Options
    {
        QFont   m_font;
        QColor  m_fontColor;
        QColor  m_backgroundColor;
    };



public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    ~OptionsDialog();

    Options options() const;

private:
    void initOptions();

private slots:
    void on_fontConfigButton_clicked();

    void on_applyButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();



private:
    Ui::OptionsDialog *m_ui;
    Options m_currentOptions;
    Options m_uncertainOptions;
    bool m_isChanged = false;
};

#endif // OPTIONS_H
