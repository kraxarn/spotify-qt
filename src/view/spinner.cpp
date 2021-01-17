#include "spinner.hpp"

Spinner::Spinner(int size, QWidget *parent)
	: size(size),
	QLabel(parent)
{
	frames = Icon::get("view-refresh").pixmap(size, size);

	setFixedSize(size, size);
	setPixmap(frame());

	timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &Spinner::timeout);
	timer->start(10);
}

QPixmap Spinner::frame()
{
	QMatrix matrix;
	matrix.rotate(angle);
	return frames.transformed(matrix,
		Qt::TransformationMode::SmoothTransformation);
}

void Spinner::timeout()
{
	setPixmap(frame());
	angle += 2;
}
