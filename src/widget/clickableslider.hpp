#pragma once

#include <QSlider>
#include <QStyleOptionSlider>
#include <QMouseEvent>

class ClickableSlider: public QSlider
{
public:
	ClickableSlider(Qt::Orientation orientation, QWidget *parent);

protected:
	void mousePressEvent(QMouseEvent *ev) override;

private:
	int valueFromPos(const QPoint &pos);
};
