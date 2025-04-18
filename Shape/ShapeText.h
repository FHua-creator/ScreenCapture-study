#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QWindow>

#include "ShapeBase.h"
#include "ShapeTextContainer.h"

class ShapeText:public ShapeBase
{
    Q_OBJECT
public:
    ShapeText(QObject* parent = nullptr);
    ~ShapeText();
public:
    QColor color{ Qt::red };
    qreal fontSize;
    bool bold;
    bool italic;
    ShapeTextContainer* container;
protected:
    void paint(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    bool mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
private:
private:
};
