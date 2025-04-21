#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include "ShapeRectBase.h"

class ShapeMosaicRect:public ShapeRectBase
{
    Q_OBJECT
public:
    ShapeMosaicRect(QObject* parent = nullptr);
    ~ShapeMosaicRect();
    virtual void paint(QPainter* painter) override;
    bool mouseRelease(QMouseEvent* event) override;
private:
private:
    QImage imgPatch;
    QImage mosaicPixs;
    int mosaicRectSize{ 18 };
};
