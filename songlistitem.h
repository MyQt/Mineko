#ifndef SONGLISTITEM_H
#define SONGLISTITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class SongListItem : public QWidget
{
    Q_OBJECT
public:
    explicit SongListItem(QWidget *parent = 0);

    void displayItem(int nIndex, QPixmap &picRadio, QString &aristName, QString &songName);
    int getId() const { return m_nIndex; }

signals:

public slots:
private:
    QLabel* m_pic;
    QLabel* m_singer;
    QLabel* m_song;
    QHBoxLayout m_layout;
private:
    int m_nIndex;
};

#endif // SONGLISTITEM_H
