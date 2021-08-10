#include "hiddensizegrip.hpp"
#include "mainwindow.hpp"

HiddenSizeGrip::HiddenSizeGrip(const std::function<QPoint(const QRect &)> &position,
	QWidget *parent)
	: QSizeGrip(parent)
{
	auto *mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		return;
	}

	resize(size, size);
	move(position(mainWindow->geometry()) - position(geometry()));
}

void HiddenSizeGrip::paintEvent(QPaintEvent */*event*/)
{
}
