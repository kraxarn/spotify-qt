#include "configview.hpp"

ConfigView::ConfigView(const lib::settings &settings, QWidget *parent)
	: QWidget(parent),
	settings(settings)
{
	auto *layout = new QVBoxLayout(this);
	tree = new QTreeWidget(this);
	layout->addWidget(tree, 1);

	tree->setHeaderLabels({
		"Key",
		"Value"
	});

	tree->header()->setSectionsMovable(false);
	tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	tree->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(tree, &QWidget::customContextMenuRequested,
		this, &ConfigView::onMenuRequested);

	footer = new QHBoxLayout();
	layout->addLayout(footer);

	auto *notice = new QLabel(QStringLiteral(
			"Account information, and other sensitive information, "
			"is automatically removed from the output"),
		this);
	notice->setWordWrap(true);
	footer->addWidget(notice, 1);

	auto *copyToClipboard = new QPushButton(QStringLiteral("Copy to clipboard"), this);
	QPushButton::connect(copyToClipboard, &QPushButton::clicked,
		this, &ConfigView::onCopyToClipboard);
	footer->addWidget(copyToClipboard);
}

void ConfigView::onMenuRequested(const QPoint &pos)
{
	auto *menu = new QMenu(this);
	auto *item = tree->itemAt(pos);

	if (item != nullptr)
	{
		auto *copyKey = menu->addAction(Icon::get(QStringLiteral("edit-copy")),
			QStringLiteral("Copy key"));

		QAction::connect(copyKey, &QAction::triggered, [item](bool /*checked*/)
		{
			QApplication::clipboard()->setText(item->text(0));
		});

		auto *copyValue = menu->addAction(Icon::get(QStringLiteral("edit-copy")),
			QStringLiteral("Copy value"));

		QAction::connect(copyValue, &QAction::triggered, [item](bool /*checked*/)
		{
			QApplication::clipboard()->setText(item->text(1));
		});

		menu->addSeparator();
	}

	QAction::connect(menu->addAction(Icon::get("folder-txt"), "Open in external editor"),
		&QAction::triggered, [this](bool /*checked*/)
		{
			Url::open(this->settings.file_name(), LinkType::Path, this);
		});

	menu->popup(tree->mapToGlobal(pos));
}

void ConfigView::reload()
{
	tree->clear();

	const auto &json = settings.to_json();
	for (const auto &i: json.items())
	{
		auto *item = new QTreeWidgetItem(tree);
		item->setText(0, QString::fromStdString(i.key()));

		for (const auto &ii: i.value().items())
		{
			auto *child = new QTreeWidgetItem(item);
			child->setText(0, QString::fromStdString(ii.key()));
			child->setText(1, QString::fromStdString(ii.value().dump()));
			child->setToolTip(1, ii.value().type_name());
		}
	}
}

void ConfigView::showEvent(QShowEvent * /*event*/)
{
	reload();
}

auto ConfigView::safeSettings() -> QString
{
	auto copy = settings.to_json();

	// Account
	copy.erase("Account");

	// General
	auto &general = copy.at("General");
	general.erase("custom_playlist_order");
	general.erase("last_device");
	general.erase("last_playlist");

	// Spotify
	auto &spotify = copy.at("Spotify");
	spotify.erase("path");
	spotify.erase("username");

	return QString::fromStdString(copy.dump(4));
}

void ConfigView::onCopyToClipboard(bool /*checked*/)
{
	QApplication::clipboard()->setText(safeSettings());
}
