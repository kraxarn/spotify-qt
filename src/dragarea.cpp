#include "dragarea.hpp"

#include "mainwindow.hpp"
#include "lib/log.hpp"

DragArea::DragArea(QWidget *parent)
	: mainWindow(MainWindow::find(parent)),
	QWidget(parent)
{
	if (mainWindow == nullptr)
	{
		lib::log::error("DragArea: MainWindow not found");
	}
}

void DragArea::mousePressEvent(QMouseEvent *event)
{
	dragPosition = event->globalPos() - mainWindow->frameGeometry().topLeft();
	event->accept();
}

void DragArea::mouseMoveEvent(QMouseEvent *event)
{
	setCursor(Qt::SizeAllCursor);
	mainWindow->move(event->globalPos() - dragPosition);
	event->accept();
}

void DragArea::mouseReleaseEvent(QMouseEvent */*event*/)
{
	setCursor(Qt::ArrowCursor);
}
