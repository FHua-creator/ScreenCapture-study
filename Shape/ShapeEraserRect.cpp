﻿#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserRect.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"


ShapeEraserRect::ShapeEraserRect(QObject* parent) : ShapeRectBase(parent)
{
    isFill = true;
    isEraser = true;
}

ShapeEraserRect::~ShapeEraserRect()
{
}

void ShapeEraserRect::paint(QPainter* painter)
{
    painter->setBrush(Qt::transparent);
    painter->setPen(Qt::NoPen);
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    painter->drawRect(shape);
    painter->restore();

}

void ShapeEraserRect::paintDragger(QPainter* painter)
{
    ShapeRectBase::paintDragger(painter);
	QPen pen;
	pen.setColor(Qt::black);
	pen.setWidth(1);
    pen.setStyle(Qt::CustomDashLine);
    pen.setDashPattern({ 3,3 });
	painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
	painter->drawRect(shape);
}
