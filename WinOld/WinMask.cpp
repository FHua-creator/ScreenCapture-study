﻿#include <QWindow>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <QTimer>

#include "WinMask.h"
#include "WinFull.h"
#include "../App/NativeRect.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/PixelInfo.h"

WinMask::WinMask(QObject* parent) : WinBase(parent)
{
    auto win = (WinFull*)parent;
    x = win->x; y = win->y;
    w = win->w; h = win->h;
    maskStroke = maskStroke * win->dpr;
    initWindow();
    show();
}

WinMask::~WinMask()
{
}

void WinMask::mousePress(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    if (win->state == State::start)
    {
        posPress = event->pos();
        win->state = State::mask;
        if(win->pixelInfo) win->pixelInfo->close();
        event->accept();
        return;
    }
    if (win->state == State::tool)
    {
        posPress = event->pos();
        win->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(posPress);
        }
        event->accept();
        return;
    }
    if (win->state > State::tool && mousePosState > 0) {
        posPress = event->pos();
        win->hideTools(win->state);
        event->accept();
        return;
    }
}

void WinMask::mouseDrag(QMouseEvent* event)
{
    auto father = (WinFull*)parent();
    if (father->state == State::mask)
    {
        auto pos = event->pos();
		if (pos == posPress) return;
        maskRect.setCoords(posPress.x(), posPress.y(), pos.x(),pos.y());
        maskRect = maskRect.normalized();
        update();
        return;
    }
    if (father->state == State::tool)
    {
        if (mousePosState == 0)
        {
            moveMaskRect(event->pos());
            update();
        }
        else
        {
            auto pos = event->pos();
            changeMaskRect(pos);
        }
        return;
    }
    if (father->state > State::tool && mousePosState > 0) {
        auto pos = event->pos();
        changeMaskRect(pos);
        return;
    }
}

void WinMask::mouseRelease(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    if (win->state == State::mask || (win->state >= State::tool && !win->toolMain->isVisible()) )
    {
        win->state = State::tool;
        win->showToolMain();
        update();
    }
}

void WinMask::mouseMove(QMouseEvent* event)
{
    auto father = (WinFull*)parent();
    if (father->state == State::start)
    {
        QGuiApplication::setOverrideCursor(Qt::CrossCursor);
        auto& rects = NativeRect::getWinRects();
        for (int i = 0; i < rects.size(); i++)
        {
            if (rects[i].contains(event->pos())) {
                if (maskRect == rects[i]) return;
                maskRect = rects[i];
                update();
                return;
            }
        }
    }
    else if (father->state == State::tool)
    {
        auto pos = event->pos();
        changeMousePosState(pos.x(), pos.y());
        event->accept();
    }
    else if (father->state > State::tool) {
        auto pos = event->pos();
        changeMousePosState2(pos.x(), pos.y());
        if (mousePosState != -1) {
            event->accept();
            return;
        }
    }
}
void WinMask::update()
{
    if (img.isNull()) {
        img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    }
    //绘制半透明和透明区域
    img.fill(QColor(0, 0, 0, 120));
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.setBrush(Qt::transparent);
    p.drawRect(maskRect);
	paintMaskRectBorder(p);
    paintMaskRectInfo(p);
    paint();
    p.end();
    auto win = (WinFull*)parent();
    if (win->state == State::tool) {
        releaseImg();
    }
}
void WinMask::changeMaskRect(const QPoint& pos)
{
    if (mousePosState == 1)
    {
        maskRect.setTopLeft(pos);
    }
    else if (mousePosState == 2)
    {
        maskRect.setTop(pos.y());
    }
    else if (mousePosState == 3)
    {
        maskRect.setTopRight(pos);
    }
    else if (mousePosState == 4)
    {
        maskRect.setRight(pos.x());
    }
    else if (mousePosState == 5)
    {
        maskRect.setBottomRight(pos);
    }
    else if (mousePosState == 6)
    {
        maskRect.setBottom(pos.y());
    }
    else if (mousePosState == 7)
    {
        maskRect.setBottomLeft(pos);
    }
    else if (mousePosState == 8)
    {
        maskRect.setLeft(pos.x());
    }
    maskRect = maskRect.normalized();
    update();
}
void WinMask::changeMousePosState(const int& x, const int& y)
{
    auto leftX = maskRect.left(); auto topY = maskRect.top();
    auto rightX = maskRect.right(); auto bottomY = maskRect.bottom();
    if (x>leftX+1 && y>topY+1 && x<rightX-1 && y<bottomY-1) //不然截图区域上顶天，下顶地的时候没法改变高度
    {
        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x <= leftX && y <= topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x <= rightX && y <= topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y <= topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y <= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x <= rightX && y >= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x <= leftX && y >= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x <= leftX && y <= bottomY && y >= topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
}
void WinMask::changeMousePosState2(const int& x, const int& y)
{
    auto x1{ maskRect.x() - maskStroke }, x2{ x1 + maskStroke * 3 };
    auto x3{ maskRect.right() - maskStroke }, x4{ x3 + maskStroke * 3 };
    auto y1{ maskRect.y() - maskStroke }, y2{ y1 + maskStroke * 3 };
    auto y3{ maskRect.bottom() - maskStroke }, y4{ y3 + maskStroke * 3 };
    if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x <= x3 && y >= y1 && y <= y2) {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
    else {
        QGuiApplication::restoreOverrideCursor();
        mousePosState = -1;
    }
}
void WinMask::paintMaskRectInfo(QPainter& p)
{
    //绘制截图区域位置和大小
    auto text = QString("X:%1 Y:%2 R:%3 B:%4 W:%5 H:%6")
        .arg(maskRect.x()).arg(maskRect.y())
        .arg(maskRect.right()).arg(maskRect.bottom())
        .arg(maskRect.width()).arg(maskRect.height());
    auto font = p.font();
    font.setPointSizeF(12.f);
    p.setFont(font);
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(text);
    int w = textRect.width();
    int h = textRect.height();

	auto y = maskRect.y() - h - 12;
    auto x = maskRect.x() + 4;
	if (y < 0) y = maskRect.y() + 4;
    QRect rect(x, y, w + 16, h + 8);


    p.setBrush(QColor(0, 0, 0, 120));
    p.setPen(Qt::NoPen);
    p.drawRect(rect);
    p.setPen(QPen(QBrush(Qt::white), 1));
    p.setBrush(Qt::NoBrush);
    p.drawText(rect, Qt::AlignCenter, text);
}

void WinMask::paintMaskRectBorder(QPainter& p)
{
    //绘制透明区域的边框
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QColor borderColor(22, 118, 255);
    p.setPen(QPen(QBrush(borderColor), maskStroke));
    p.setBrush(Qt::NoBrush);
    p.drawRect(maskRect);
    auto win = (WinFull*)parent();
    if (win->state == State::tool) {
        //绘制边框上的拖动圆点
        p.setBrush(borderColor);
        p.setPen(Qt::NoPen);
        auto r{ 2 * maskStroke };
        auto hw{ maskRect.width() / 2 };
        auto hh{ maskRect.height() / 2 };
        p.drawEllipse(maskRect.topLeft().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.left() + hw, maskRect.top()), r, r);
        p.drawEllipse(maskRect.topRight().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.right(), maskRect.top() + hh), r, r);
        p.drawEllipse(maskRect.bottomRight().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.left() + hw, maskRect.bottom()), r, r);
        p.drawEllipse(maskRect.bottomLeft().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.left(), maskRect.top() + hh), r, r);
    }
}

void WinMask::moveMaskRect(const QPoint& pos)
{
	auto span = pos - posPress;
	auto target = maskRect.topLeft()+span;
	posPress = pos;
	if (target.x() < 0 || target.y()<0) return;
	if (target.x() + maskRect.width() > w || target.y() + maskRect.height() > h) return;
    maskRect.moveTo(maskRect.topLeft() + span);
	update();
}


