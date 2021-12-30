#pragma once

#include <QSlider>
#include <QStyleOptionSlider>
#include <QMouseEvent>

class ClickableSlider: public QSlider
{
public:
	ClickableSlider(Qt::Orientation orientation, QWidget *parent);

	/**
	 * Enable/disable custom click handling
	 */
	void setUpdateOnClickEnabled(bool value);

protected:
	void mousePressEvent(QMouseEvent *event) override;

private:
	bool enabled = true;

	auto valueFromPos(const QPoint &pos) -> int;
};
