#include "widget/position.hpp"

#include <QHBoxLayout>

Position::Position(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);
	layout->setContentsMargins({});
	layout->setSpacing(2);

	currentTime = new Time(this);
	layout->addWidget(currentTime);

	auto *seperator = new QLabel(QStringLiteral("/"), this);
	seperator->setAlignment(seperator->alignment() | Qt::AlignHCenter);
	layout->addWidget(seperator);

	totalTime = new Time(this);
	layout->addWidget(totalTime);
}

void Position::setCurrent(int milliseconds)
{
	currentTime->setTime(milliseconds);
}

void Position::setTotal(int milliseconds)
{
	totalTime->setTime(milliseconds);
}
