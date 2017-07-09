#ifndef LOADING_H
#define LOADING_H

#include <QFrame>
#include <QWidget>

namespace Ui {
class loading;
}

class loading : public QFrame
{
    Q_OBJECT

public:
    explicit loading(QWidget *parent = 0);
    ~loading();

private:
    Ui::loading *ui;
};

#endif // LOADING_H
