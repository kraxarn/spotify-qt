#include "clienthandlerlogview.hpp"

ClientHandlerLogView::ClientHandlerLogView(QWidget *parent)
	: QTreeWidget(parent)
{
	setHeaderLabels({
		"Time",
		"Message",
	});
	setRootIsDecorated(false);
}

void ClientHandlerLogView::showEvent(QShowEvent */*event*/)
{
	clear();
	for (const auto &line : spt::ClientHandler::getLog())
	{
		auto *item = new QTreeWidgetItem(this);
		item->setText(0, line.first.time().toString());
		item->setText(1, line.second);
	}
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &ClientHandlerLogView::menu);

	header()->resizeSections(QHeaderView::ResizeToContents);
}

void ClientHandlerLogView::menu(const QPoint &pos)
{
	auto *menu = new QMenu(this);
	QAction::connect(menu->addAction("Copy log to clipboard"), &QAction::triggered,
		[](bool /*checked*/)
		{
			QStringList list;
			for (const auto &line : spt::ClientHandler::getLog())
			{
				list.append(QString("[%1] %2").arg(line.first.time().toString(),
					line.second));
			}
			QApplication::clipboard()->setText(list.join('\n'));
		});
	menu->popup(mapToGlobal(pos));
}
