#include "optionsdialog.h"
#include "ui_optionsdialog.h"

#include <QFontDialog>
#include <QColorDialog>



OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OptionsDialog)
{
    m_ui->setupUi(this);

    // 配置Tab栏 “Font”
    m_currentOptions.m_font = m_ui->fontLineEdit->font();
    m_currentOptions.m_font.setStyleName("Regular");
    m_currentOptions.m_fontColor.setRgb(0, 0, 0);
    m_currentOptions.m_backgroundColor.setRgb(255, 255, 255);
    m_uncertainOptions = m_currentOptions;


    initOptions();
}

void OptionsDialog::initOptions()
{
    m_ui->fontLineEdit->setFont(m_currentOptions.m_font);
    QString str =   m_currentOptions.m_font.family() + ", " +
                    m_currentOptions.m_font.styleName() +
                    ", " + QString::number(m_currentOptions.m_font.pointSize()) + "pt";
    m_ui->fontLineEdit->setText(str);

    str = "background-color: rgb(%1, %2, %3);";
    m_ui->textColorToolButton->setStyleSheet(str.arg(m_currentOptions.m_fontColor.red()).
                                             arg(m_currentOptions.m_fontColor.green()).
                                             arg(m_currentOptions.m_fontColor.blue())
                                             );

    str = "background-color: rgb(%1, %2, %3);";
    m_ui->bgColorToolButton->setStyleSheet(str.arg(m_currentOptions.m_backgroundColor.red()).
                                             arg(m_currentOptions.m_backgroundColor.green()).
                                             arg(m_currentOptions.m_backgroundColor.blue())
                                             );
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
    QFont font = QFontDialog::getFont(&ok, m_currentOptions.m_font, nullptr, tr("Font"));
    if (ok)
    {
        //m_ui->fontLineEdit->setFont(font);
        QString str = font.family() + ", " + font.styleName() + ", " + QString::number(font.pointSize()) + "pt";
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
        initOptions();
        accept();
    }
    else
    {
        initOptions();
        reject();
    }

}

void OptionsDialog::on_applyButton_clicked()
{
    m_currentOptions = m_uncertainOptions;
    m_isChanged = true;
}

void OptionsDialog::on_textColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(m_currentOptions.m_fontColor, nullptr,
                                          tr("Text Color"), QColorDialog::ShowAlphaChannel);
    if (color != QColor::Invalid)
    {
        QString str = "background-color: rgb(%1, %2, %3);";
        m_ui->textColorToolButton->setStyleSheet(str.arg(color.red()).
                                                 arg(color.green()).
                                                 arg(color.blue())
                                                 );

        m_uncertainOptions.m_fontColor = color;
    }


}

void OptionsDialog::on_bgColorToolButton_clicked()
{
    QColor color = QColorDialog::getColor(m_currentOptions.m_backgroundColor, nullptr,
                                          tr("Background Color"), QColorDialog::ShowAlphaChannel);
    if (color != QColor::Invalid)
    {
        QString str = "background-color: rgb(%1, %2, %3);";
        m_ui->bgColorToolButton->setStyleSheet(str.arg(color.red()).
                                                 arg(color.green()).
                                                 arg(color.blue())
                                                 );

        m_uncertainOptions.m_backgroundColor = color;
    }
}

void OptionsDialog::on_defaultColorButton_clicked()
{
    m_uncertainOptions.m_fontColor.setRgb(0, 0, 0);
    m_uncertainOptions.m_backgroundColor.setRgb(255, 255, 255);

    QString str = "background-color: rgb(%1, %2, %3);";
    m_ui->textColorToolButton->setStyleSheet(str.arg(m_uncertainOptions.m_fontColor.red()).
                                             arg(m_uncertainOptions.m_fontColor.green()).
                                             arg(m_uncertainOptions.m_fontColor.blue())
                                             );

    str = "background-color: rgb(%1, %2, %3);";
    m_ui->bgColorToolButton->setStyleSheet(str.arg(m_uncertainOptions.m_backgroundColor.red()).
                                             arg(m_uncertainOptions.m_backgroundColor.green()).
                                             arg(m_uncertainOptions.m_backgroundColor.blue())
                                             );
}
