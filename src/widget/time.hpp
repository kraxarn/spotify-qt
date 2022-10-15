#pragma once

#include <QLabel>

class Time: public QWidget
{
Q_OBJECT

public:
	explicit Time(QWidget *parent);

	void setTime(int milliseconds);

private:
	static constexpr int labelCount = 5;

	auto labelAt(int index) -> QLabel *;
};
