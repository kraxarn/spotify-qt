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

void DragArea::mouseDoubleClickEvent(QMouseEvent *event)
{
	onMaximize(false);
}

void DragArea::menu(const QPoint &pos)
{
	auto *menu = new QMenu(this);

	auto *minimize = menu->addAction(Icon::get("window-minimize-symbolic"),
		"Minimize");
	QAction::connect(minimize, &QAction::triggered, [this](bool /*checked*/)
	{
		mainWindow->setWindowState(Qt::WindowMinimized);
	});

	const auto isMaximized = isWindowMaximized();
	auto *maximize = menu->addAction(Icon::get(isMaximized
			? "window-restore"
			: "window-maximize-symbolic"),
		isMaximized
			? "Restore"
			: "Maximize");

	QAction::connect(maximize, &QAction::triggered,
		this, &DragArea::onMaximize);

	auto *close = menu->addAction(Icon::get("window-close-symbolic"),
		"Close");
	QAction::connect(close, &QAction::triggered, &QCoreApplication::quit);

	menu->popup(mapToGlobal(pos));
}

auto DragArea::isWindowMaximized() -> bool
{
	return mainWindow->windowState() == Qt::WindowMaximized;
}

void DragArea::onMaximize(bool /*checked*/)
{
	mainWindow->setWindowState(isWindowMaximized()
		? Qt::WindowNoState
		: Qt::WindowMaximized);
}
