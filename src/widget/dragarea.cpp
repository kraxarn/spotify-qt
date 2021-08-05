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

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &DragArea::menu);
}

void DragArea::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
	{
		return QWidget::mousePressEvent(event);
	}

	dragPosition = event->globalPos() - mainWindow->frameGeometry().topLeft();
	event->accept();
}

void DragArea::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
		return QWidget::mouseMoveEvent(event);
	}

	setCursor(Qt::SizeAllCursor);
	mainWindow->move(event->globalPos() - dragPosition);
	event->accept();
}

void DragArea::mouseReleaseEvent(QMouseEvent */*event*/)
{
	setCursor(Qt::ArrowCursor);
}

void DragArea::menu(const QPoint &pos)
{
	auto *menu = new QMenu(this);

	auto *minimize = menu->addAction(Icon::get("window-minimize-symbolic"),
		"Minimize");
	QAction::connect(minimize, &QAction::triggered, [this](bool /*checked*/)
	{
		emit this->mainWindow->showMinimized();
	});

	const auto isMaximized = mainWindow->isMaximized();
	auto *maximize = menu->addAction(Icon::get(isMaximized
			? "window-restore"
			: "window-maximize-symbolic"),
		isMaximized
			? "Restore"
			: "Maximize");

	QAction::connect(maximize, &QAction::triggered, [this](bool /*checked*/)
	{
		if (mainWindow->isMaximized())
		{
			mainWindow->resize(MainWindow::defaultSize());
		}
		else
		{
			emit mainWindow->showMaximized();
		}
	});

	auto *close = menu->addAction(Icon::get("window-close-symbolic"),
		"Close");
	QAction::connect(close, &QAction::triggered, &QCoreApplication::quit);

	menu->popup(mapToGlobal(pos));
}
