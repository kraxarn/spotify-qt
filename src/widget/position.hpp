#pragma once

#include "widget/time.hpp"

class Position: public QWidget
{
Q_OBJECT

public:
	explicit Position(QWidget *parent);

	void setCurrent(int milliseconds);
	void setTotal(int milliseconds);

private:
	Time *currentTime;
	Time *totalTime;
};
