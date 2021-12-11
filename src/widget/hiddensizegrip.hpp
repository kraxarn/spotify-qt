#pragma once

#include <QSizeGrip>

#include <functional>

class HiddenSizeGrip: public QSizeGrip
{
public:
	HiddenSizeGrip(const std::function<QPoint(const QRect &)> &position, QWidget *parent);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	static constexpr int size = 12;
};
