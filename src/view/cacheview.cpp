#include "cacheview.hpp"

CacheView::CacheView(const QString &cachePath, QWidget *parent)
	: cachePath(cachePath),
	QTreeWidget(parent)
{
	setHeaderLabels({
		"Folder",
		"Files",
		"Size",
	});
	setRootIsDecorated(false);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested, this, &CacheView::menu);
}

QString CacheView::fullName(const QString &folderName)
{
	if (folderName == "album")
		return "Album covers";
	if (folderName == "librespot")
		return "Librespot cache";
	if (folderName == "playlist")
		return "Playlists";
	if (folderName == "qmlcache")
		return "spotify-qt-quick cache";
	if (folderName == "tracks")
		return "Album and library";

	return folderName;
}

void CacheView::folderSize(const QString &path, unsigned int *count, unsigned int *size)
{
	for (auto &file : QDir(path).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files))
	{
		if (file.isDir())
		{
			folderSize(file.absoluteFilePath(), count, size);
			continue;
		}

		(*count)++;
		*size += file.size();
	}
}

void CacheView::menu(const QPoint &pos)
{
	auto item = itemAt(pos);
	auto folder = item->data(0, 0x100).toString();
	if (folder.isEmpty())
		return;

	auto menu = new QMenu(this);
	QAction::connect(menu->addAction(Icon::get("folder-temp"), "Open folder"),
		&QAction::triggered, [this, folder](bool)
		{
			Utils::openUrl(folder, LinkType::Path, this);
		});
	menu->popup(mapToGlobal(pos));
}

void CacheView::reload()
{
	clear();

	for (auto &dir : QDir(cachePath).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		auto item = new QTreeWidgetItem(this);
		item->setText(0, fullName(dir.baseName()));

		auto count = 0u;
		auto size = 0u;
		folderSize(dir.absoluteFilePath(), &count, &size);

		item->setData(0, 0x100, dir.absoluteFilePath());
		item->setText(1, QString::number(count));
		item->setText(2, Utils::formatSize(size));
	}

	header()->resizeSections(QHeaderView::ResizeToContents);
}

void CacheView::showEvent(QShowEvent *)
{
	reload();
}
