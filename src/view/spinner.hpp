#pragma once

#include "../util/icon.hpp"

#include <QLabel>

class Spinner: public QLabel
{
public:
	explicit Spinner(int size = 22, QWidget *parent = nullptr);

private:
	int size = 0;
	double angle = 0;
	QPixmap frames;
	QTimer *timer = nullptr;

	QPixmap frame();
	void timeout();
};
