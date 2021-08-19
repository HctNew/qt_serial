#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE

class QFont;
class QColor;
class QDomDocument;
class QDomElement;

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
        QColor  m_textColor;
        QColor  m_backgroundColor;
    };

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    ~OptionsDialog();

    Options options() const { return m_currentOptions; };

    bool xmlInitOptions(const QString xmlFile);
    bool xmlSaveOptions(const QString xmlFile);
    bool xmlLoadOptions(const QString xmlFile);

private:
    void updateOptions();
    void xmlInitDisplaySettings(QDomElement &parentElem, QDomDocument & doc);
    void xmlSaveDisPlaySettings(QDomElement &parentElem);
    void xmlLoadDisPlaySettings(const QDomElement &parentElem);

private slots:
    void on_fontConfigButton_clicked();

    void on_applyButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_textColorToolButton_clicked();

    void on_bgColorToolButton_clicked();

    void on_defaultColorButton_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::OptionsDialog *m_ui;
    Options m_currentOptions;
    Options m_uncertainOptions;
    bool m_isChanged = false;
};

#endif // OPTIONS_H
