#pragma once
#include <QList>
#include <QRect>
class NativeRect
{
public:
	static void init();
	static const QList<QRect>& getWinRects();
	static const QList<QRect>& getScreenRects();
	static const QRect& getDesktopRect();
private:
	static void initWinRect();
	static void initScreenRect();
	static void initDesktopRect();
};

