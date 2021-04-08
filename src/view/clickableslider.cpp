#include "clickableslider.hpp"

ClickableSlider::ClickableSlider(Qt::Orientation orientation, QWidget *parent)
	: QSlider(orientation, parent)
{
}

void ClickableSlider::mousePressEvent(QMouseEvent *ev)
{
	QSlider::mousePressEvent(ev);

	if (ev->button() == Qt::LeftButton)
	{
		setValue(valueFromPos(ev->pos()));
	}
	emit sliderReleased();
}

int ClickableSlider::valueFromPos(const QPoint &pos)
{
	QStyleOptionSlider styleOption;
	initStyleOption(&styleOption);

	auto groove = style()->subControlRect(QStyle::CC_Slider, &styleOption,
		QStyle::SC_SliderGroove, this);
	auto handle = style()->subControlRect(QStyle::CC_Slider, &styleOption,
		QStyle::SC_SliderHandle, this);

	int sliderLength;
	int sliderMin;
	int sliderMax;

	if (orientation() == Qt::Horizontal)
	{
		sliderLength = handle.width();
		sliderMin = groove.x();
		sliderMax = groove.right() - sliderLength + 1;
	}
	else
	{
		sliderLength = handle.height();
		sliderMin = groove.y();
		sliderMax = groove.bottom() - sliderLength + 1;
	}

	auto posRect = pos - handle.center() + handle.topLeft();
	auto posVal = orientation() == Qt::Horizontal ? posRect.x() : posRect.y();
	return QStyle::sliderValueFromPosition(minimum(), maximum(), posVal - sliderMin,
		sliderMax - sliderMin, styleOption.upsideDown);
}
