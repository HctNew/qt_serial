#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QFontDialog>



OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OptionsDialog)
{
    m_ui->setupUi(this);

    // 配置Tab栏 “Font”
    m_currentOptions.m_font = m_ui->fontLineEdit->font();
    m_currentOptions.m_font.setStyleName("Regular");
    //m_currentOptions.m_fontColor =


    initOptions();
}

void OptionsDialog::initOptions()
{
    m_ui->fontLineEdit->setFont(m_currentOptions.m_font);
    QString str =   m_currentOptions.m_font.family() + ", " +
                    m_currentOptions.m_font.styleName() +
                    tr(", %1").arg(m_currentOptions.m_font.pointSize());
    m_ui->fontLineEdit->setText(str);
}

OptionsDialog::~OptionsDialog()
{
    delete m_ui;
}

OptionsDialog::Options OptionsDialog::options() const
{
    return m_currentOptions;
}

void OptionsDialog::on_fontConfigButton_clicked()
{
    bool ok = true;
    QFont font = QFontDialog::getFont(&ok);
    if (ok)
    {
        m_ui->fontLineEdit->setFont(font);
        QString str = font.family() + ", " + font.styleName() + tr(", %1").arg(font.pointSize());
        m_ui->fontLineEdit->setText(str);
        m_uncertainOptions.m_font = font;
    }

}


void OptionsDialog::on_okButton_clicked()
{
    m_currentOptions = m_uncertainOptions;

    accept();
}

void OptionsDialog::on_cancelButton_clicked()
{
    if (m_isChanged)
    {
        m_isChanged = false;
        m_ui->fontLineEdit->setFont(m_currentOptions.m_font);
        initOptions();
        accept();
    }
    else
    {
        reject();
    }

}

void OptionsDialog::on_applyButton_clicked()
{
    m_currentOptions = m_uncertainOptions;
    m_isChanged = true;
}
