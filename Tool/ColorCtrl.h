#pragma once

#include <QWidget>


class ColorCtrl  : public QWidget
{
	Q_OBJECT

public:
	ColorCtrl(QWidget *parent);
	ColorCtrl(int selectIndex, QWidget* parent);
	~ColorCtrl();
	static void initData(const QJsonObject& obj, const QString& lang);
    QColor getColor();
	int selectedIndex{ 0 };
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
private:
	int itemWidth{ 26 };
	int hoverIndex{ -1 };
};
