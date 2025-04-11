#include <QApplication>
#include <Windows.h>
#include <qscreen.h>
#include <qimage.h>
#include <qwindow.h>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>

#include "../App/App.h"
#include "../App/NativeRect.h"
#include "WinFull.h"
#include "WinPin.h"
#include "WinMask.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "../Shape/ShapeBase.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/PixelInfo.h"
#include "../App/Util.h"

WinFull::WinFull(QObject* parent) : WinBox(parent)
{
    auto rect = NativeRect::getDesktopRect();
    x = rect.x(); y = rect.y();
    w = rect.width();h = rect.height();
    img = Util::printScreen(x, y, w, h);
    initWindow(false);
    show();
}
WinFull::~WinFull()
{
    
}
void WinFull::init()
{
    auto winFull = new WinFull();
    winFull->winBoard = new WinBoard(winFull);
    winFull->winCanvas = new WinCanvas(winFull);
    winFull->winMask = new WinMask(winFull);
    winFull->pixelInfo = new PixelInfo(winFull);
    SetFocus(winFull->hwnd);
}
void WinFull::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    toolMain->setBtnEnable(QString{ "pin" }, true);
    toolMain->confirmPos();
    toolMain->show();
}
void WinFull::showToolSub()
{
    if (!toolSub) {
        toolSub = new ToolSub(this);
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    toolSub->show();
}

void WinFull::close()
{
    if (pixelInfo) {
        pixelInfo->close();
    }
    if (winMask) {
        winMask->close();
    }
    if (winCanvas) {
        winCanvas->close();
    }
    if (winBoard) {
        winBoard->close();
    }
    if (toolMain) {
        toolMain->close();
    }
    if (toolSub) {
        toolSub->close();
    }
    WinBase::close();
}

QImage WinFull::getCutImg()
{
    QImage tar = img.copy(winMask->maskRect);
    QPainter p(&tar);
    p.translate(0 - winMask->maskRect.left(), 0 - winMask->maskRect.top());
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto shape : shapes)
    {
        shape->paint(&p);
    }
	return tar; 
}

void WinFull::saveToClipboard()
{
    Util::imgToClipboard(getCutImg());
    close();
}

void WinFull::saveToFile()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    auto filePath = QDir::cleanPath(desktopPath + QDir::separator() + "Img" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png");
    filePath = QFileDialog::getSaveFileName(toolMain, tr("保存文件"), filePath, "ScreenCapture (*.png)");
    if (filePath.isEmpty())
    {
        return;
    }
    getCutImg().save(filePath);
    close();
}

void WinFull::mousePress(QMouseEvent* event)
{
    event->ignore();
    winMask->mousePress(event);
    mousePressOnShape(event);
}

void WinFull::mouseMove(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseMove(event);
    if (event->isAccepted()) return;
    if (state == State::start && pixelInfo) {
        pixelInfo->mouseMove(event->pos());
        return;
    }
    mouseMoveOnShape(event);
    if (!event->isAccepted()) {
        if (state == State::text) {
            QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
        }
        else {
            QGuiApplication::setOverrideCursor(Qt::CrossCursor);
        }
    }
}

void WinFull::mouseDrag(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseDrag(event);
    mouseDragOnShape(event);
}

void WinFull::mouseRelease(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseRelease(event);
    mouseReleaseOnShape(event);
}
void WinFull::mouseDBClick(QMouseEvent* event)
{
    saveToClipboard();
}
void WinFull::mousePressRight(QMouseEvent* event)
{
	escPress();
}
void WinFull::moveByKey(const int& key)
{
    if (state == State::start) {
        POINT point;
        GetCursorPos(&point);
        if (key == 0) SetCursorPos(point.x - 1, point.y);
        else if (key == 1) SetCursorPos(point.x, point.y - 1);
        else if (key == 2) SetCursorPos(point.x + 1, point.y);
        else if (key == 3) SetCursorPos(point.x, point.y + 1);
    }
    else {
        SetFocus(hwnd);
        hideTools(state);
        if (key == 0) winMask->maskRect.translate(-1, 0);
        else if (key == 1) winMask->maskRect.translate(0, -1);
        else if (key == 2) winMask->maskRect.translate(1, 0);
        else if (key == 3) winMask->maskRect.translate(0, 1);
        showToolMain();
        winMask->update();
    }
}
void WinFull::copyColor(const int& key)
{
    if (key == 3) {
        POINT pos;
        GetCursorPos(&pos);
        std::wstring tarStr = QString("%1,%2").arg(pos.x).arg(pos.y).toStdWString();
        Util::setClipboardText(tarStr);
        close();
        return;
    }
    winMask->close();
    Util::copyColor(key);
    close();
}

void WinFull::escPress()
{
	if (state > State::start) {
        hideTools();
        SetFocus(hwnd);
	}
    else {
        close();
    }
}
