#include "dragarea.hpp"
#include "mainwindow.hpp"

DragArea::DragArea(QWidget *target, QWidget *parent)
	: QWidget(parent),
	target(target)
{
	// Only show context menu on main window
	if (qobject_cast<QMainWindow *>(target) != nullptr)
	{
		setContextMenuPolicy(Qt::CustomContextMenu);
		QWidget::connect(this, &QWidget::customContextMenuRequested,
			this, &DragArea::menu);
	}
	else
	{
		setContextMenuPolicy(Qt::NoContextMenu);
	}
}

void DragArea::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton)
	{
		return QWidget::mousePressEvent(event);
	}

	dragPosition = event->globalPos() - target->frameGeometry().topLeft();
	event->accept();
}

void DragArea::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
		return QWidget::mouseMoveEvent(event);
	}

	setCursor(Qt::SizeAllCursor);
	target->move(event->globalPos() - dragPosition);
	event->accept();
}

void DragArea::mouseReleaseEvent(QMouseEvent */*event*/)
{
	setCursor(Qt::ArrowCursor);
}

void DragArea::mouseDoubleClickEvent(QMouseEvent */*event*/)
{
	onMaximize(false);
}

void DragArea::menu(const QPoint &pos)
{
	auto *menu = new QMenu(this);

	auto *minimize = menu->addAction(Icon::get("window-minimize"),
		"Minimize");
	QAction::connect(minimize, &QAction::triggered, [this](bool /*checked*/)
	{
		auto *mainWindow = dynamic_cast<MainWindow *>(target);
		if (mainWindow != nullptr)
		{
			mainWindow->minimize();
		}
	});

	const auto isMaximized = isWindowMaximized();
	auto *maximize = menu->addAction(Icon::get(isMaximized
			? "window-restore"
			: "window-maximize"),
		isMaximized
			? "Restore"
			: "Maximize");

	QAction::connect(maximize, &QAction::triggered,
		this, &DragArea::onMaximize);

	auto *close = menu->addAction(Icon::get("window-close"),
		"Close");
	QAction::connect(close, &QAction::triggered, &QCoreApplication::quit);

	menu->popup(mapToGlobal(pos));
}

auto DragArea::isWindowMaximized() -> bool
{
	return target->windowState() == Qt::WindowMaximized;
}

void DragArea::onMaximize(bool /*checked*/)
{
	target->setWindowState(isWindowMaximized()
		? Qt::WindowNoState
		: Qt::WindowMaximized);
}
