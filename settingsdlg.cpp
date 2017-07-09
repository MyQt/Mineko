#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include "customtabstyle.h"

#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QVBoxLayout>

#pragma execution_character_set("utf-8")

static const QString UPDATES_URL("https://raw.githubusercontent.com/"
                                 "nuttyartist/notes/automatic-updates-windows"
                                 "/UPDATES.json");

settingsDlg::settingsDlg(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::settingsDlg)
{
    ui->setupUi(this);
    setFrameStyle(QFrame::Box);
    ui->tabWidget->tabBar()->setAutoFillBackground(true);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
    settings.beginGroup("User");
    m_filePath = settings.value("filepath").toString();
    settings.endGroup();
    if (m_filePath.isEmpty()) m_filePath = "./download";
    ui->textFilePath->setText(m_filePath);
    m_skinPicker.initColorArray();
    initColorGrid();
    QVBoxLayout *pLayout = new QVBoxLayout();
    setLayout(pLayout);
    m_updater = new Updater();
    m_updater->setNotifyOnFinish(false);
    m_updater->setNotifyOnUpdate(false);
    m_updater->setDownloaderEnabled(false);
    m_updater->setUseCustomInstallProcedures(true);
    m_updater->setModuleVersion(qApp->applicationVersion());

    /* Check for updates */
    m_updater->checkForUpdates(UPDATES_URL);
}

settingsDlg::~settingsDlg()
{
    delete ui;
    delete m_updater;
}

void settingsDlg::on_btnChoice_clicked()
{
    QString iniDir = m_filePath;
    QDir dir;
    if (!dir.exists(iniDir)) iniDir= "./";
    m_filePath = QFileDialog::getExistingDirectory(this, "选择歌曲目录", iniDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!m_filePath.isEmpty())
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
        settings.beginGroup("User");
        settings.setValue("filepath", m_filePath);
        settings.endGroup();
        ui->textFilePath->setText(m_filePath);
        emit filepathChangeSlot(m_filePath);
    }

}


void settingsDlg::showDlg()
{
//    setGeometry((parentWidget()->width()-width())/2, (parentWidget()->height()-height())/2, width(), height());
    show();
}

void settingsDlg::initColorGrid()
{
    for(int i = 0, k = 0; i < m_skinPicker.getColorArray().size() / 3+1; i++, k += 3)
    {
        for(int j = 0; j < 3 && k+j < m_skinPicker.getColorArray().size(); j++)
        {
            QColor color =m_skinPicker.getColorArray()[k+j];
            QTableWidgetItem* pItem = new QTableWidgetItem();
            pItem->setBackgroundColor(color);
            QString selectedStyle("QTableWidget::item:selected { background-color:");
            selectedStyle.append(color.name());
            selectedStyle.append("}");
            ui->tableColor->setStyleSheet(selectedStyle);
            if (ui->tableColor->rowCount() <= i) {
                ui->tableColor->setRowCount(ui->tableColor->rowCount()+1);
            }

            ui->tableColor->setItem(i, j, pItem);
        }
    }

}

void settingsDlg::on_tableColor_itemClicked(QTableWidgetItem *item)
{
    item->setSelected(false);
    QColor color = item->backgroundColor();
    emit applySkinSignal(color);
}

void settingsDlg::on_stopButton_clicked()
{
    m_updater->cancelDownload();
}

void settingsDlg::on_openButton_clicked()
{

}
