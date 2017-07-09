#ifndef SKINPICKER_H
#define SKINPICKER_H
#include <QColor>
#include <QVector>
//--------------------------
// 颜色预定义列表
//--------------------------
class SkinPicker
{
public:
    SkinPicker();

    void initColorArray();
    inline const QVector<QColor>& getColorArray() { return m_colorArray; }

private:
    QVector<QColor> m_colorArray;

};

#endif // SKINPICKER_H
