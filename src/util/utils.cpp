#include "utils.hpp"

#include "icon.hpp"
#include "../dialog/openlinkdialog.hpp"

bool Utils::darkBackground = false;

QTreeWidgetItem *Utils::treeItemWithChildren(
	QTreeWidget *tree, const QString &name,
	const QString &toolTip, const QStringList &childrenItems)
{
	auto item = new QTreeWidgetItem(tree, {name});
	item->setToolTip(0, toolTip);
	for (auto &child : childrenItems)
		item->addChild(new QTreeWidgetItem(item, {child}));

	return item;
}

void Utils::applyPalette(lib::palette palette)
{
	QPalette p;
	switch (palette)
	{
		case lib::palette_app:
			p = QApplication::palette();
			break;

		case lib::palette_style:
			p = QApplication::style()->standardPalette();
			break;

		case lib::palette_dark:
			p = DarkPalette();
			break;
	}

	QApplication::setPalette(p);
}

QPixmap Utils::mask(const QPixmap &source, MaskShape shape, const QVariant &data)
{
	if (source.isNull())
		return source;

	auto img = source.toImage().convertToFormat(QImage::Format_ARGB32);
	QImage out(img.size(), QImage::Format_ARGB32);
	out.fill(Qt::GlobalColor::transparent);
	QPainter painter(&out);
	painter.setOpacity(0);
	painter.setBrush(Qt::white);
	painter.setPen(Qt::NoPen);
	painter.drawImage(0, 0, img);
	painter.setOpacity(1);
	QPainterPath path(QPointF(0, 0));

	QPolygonF polygon;
	switch (shape)
	{
		case MaskShape::App:
			polygon << QPointF(img.width() / 4.f, 0)
				<< QPointF(img.width(), 0)
				<< QPointF(img.width(), (img.height() / 4.f) * 3)
				<< QPointF((img.width() / 4.f) * 3, img.height())
				<< QPointF(0, img.height())
				<< QPointF(0, img.height() / 4.f);
			break;

		case MaskShape::Pie:
			switch (data.toInt() / 25)
			{
				case 0:
					polygon = QPolygonF(QRectF(img.width() / 2.f, 0,
						img.width() / 2.f, img.height() / 2.f));
					break;

				case 1:
					polygon = QPolygonF(QRectF(img.width() / 2.f, 0,
						img.width() / 2.f, img.height()));
					break;

				case 2:
					polygon << QPointF(img.width() / 2.f, 0)
						<< QPointF(img.width() / 2.f, img.height() / 2.f)
						<< QPointF(0, img.height() / 2.f)
						<< QPointF(0, img.height())
						<< QPointF(img.width(), img.height())
						<< QPointF(img.width(), 0);
					break;

				case 3:
					polygon = QPolygonF(QRectF(0, 0, img.width(), img.height()));
					break;
			}
			break;
	}

	path.addPolygon(polygon);
	painter.setClipPath(path);
	painter.drawImage(0, 0, img);
	return QPixmap::fromImage(out);
}

QWidget *Utils::layoutToWidget(QLayout *layout)
{
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QString Utils::formatTime(int ms)
{
	auto duration = QTime(0, 0).addMSecs(ms);
	return QString("%1:%2")
		.arg(duration.minute())
		.arg(duration.second() % 60, 2, 10, QChar('0'));
}

QGroupBox *Utils::createGroupBox(QVector<QWidget *> &widgets, QWidget *parent)
{
	auto group = new QGroupBox(parent);
	auto layout = new QVBoxLayout();
	for (auto &widget : widgets)
		layout->addWidget(widget);
	group->setLayout(layout);
	return group;
}

QAction *Utils::createMenuAction(
	const QString &iconName, const QString &text,
	QKeySequence::StandardKey shortcut)
{
	auto action = new QAction(Icon::get(iconName), text);
	if (shortcut != QKeySequence::UnknownKey)
		action->setShortcut(QKeySequence(shortcut));
	return action;
}

QTreeWidgetItem *Utils::treeItem(QTreeWidget *tree, const QString &key, const QString &value)
{
	return new QTreeWidgetItem(tree, {
		key, value
	});
}

void Utils::openUrl(const QString &url, LinkType linkType, QWidget *parent)
{
	if (!QDesktopServices::openUrl(QUrl(url)))
		OpenLinkDialog(url, linkType, parent).exec();
}

QString Utils::formatSize(unsigned int size)
{
	// gb
	if (size > 1000000000)
		return QString("%1 GB").arg(size / 1000000000);
	// mb
	if (size > 1000000)
		return QString("%1 MB").arg(size / 1000000);
	// kb
	if (size > 1000)
		return QString("%1 kB").arg(size / 1000);
	// b
	return QString("%1 B").arg(size);
}
