#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QFrame>
#include <QListWidget>
#include <QListWidgetItem>
#include <QColor>
#include <QButtonGroup>
#include <QDialog>
#include <QTableWidget>

#include "skinpicker.h"
#include "Updater.h"

namespace Ui {
class settingsDlg;
}

class settingsDlg : public QFrame
{
    Q_OBJECT

public:
    explicit settingsDlg(QWidget *parent = 0);
    ~settingsDlg();
    void showDlg();
    inline QString getFilepath() { return m_filePath ;}
    void initColorGrid();
private slots:
    void on_btnChoice_clicked();

    void on_tableColor_itemClicked(QTableWidgetItem *item);

    void on_stopButton_clicked();

    void on_openButton_clicked();

signals:
    void filepathChangeSlot(QString path);
    void applySkinSignal(QColor color);
private:
    Ui::settingsDlg *ui;
    QString m_filePath;
    SkinPicker m_skinPicker;
    QButtonGroup m_btnGroup;
    Updater* m_updater;
};

#endif // SETTINGSDLG_H
