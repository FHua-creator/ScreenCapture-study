#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <QMouseEvent>
#include <QImage>
#include "spdlog/spdlog.h"

class WinBox;
class Util
{
public:
	static QMouseEvent createMouseEvent(const QEvent::Type& type = QEvent::MouseMove, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
	static QMouseEvent createMouseEvent(const LPARAM& lParam, const QEvent::Type& type, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
	//static QImage printWindow(WinBox* win);
	//static QImage printWindow2(WinBox* win);
	static QImage printScreen(const int& x, const int& y, const int& w, const int& h);
	static void imgToClipboard(const QImage& img);
	static bool posInScreen(const int& x, const int& y);
	static QScreen* getScreen(const int& x, const int& y);
	static void setClipboardText(const std::wstring& text);
	static void copyColor(const int& key);
};

