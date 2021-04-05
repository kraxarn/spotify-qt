#pragma once

#include "../enum/linktype.hpp"
#include "../enum/maskshape.hpp"
#include "lib/enum/palette.hpp"
#include "../darkpalette.hpp"

#include <QAction>
#include <QApplication>
#include <QGroupBox>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QDesktopServices>

class Utils
{
public:
	static QTreeWidgetItem *treeItemWithChildren(QTreeWidget *tree, const QString &name,
		const QString &toolTip = QString(),
		const QStringList &childrenItems = QStringList(QString()));
	static QPixmap mask(const QPixmap &source, MaskShape shape = MaskShape::App,
		const QVariant &data = QVariant());
	static QWidget *layoutToWidget(QLayout *layout);
	static void applyPalette(lib::palette palette);
	static QGroupBox *createGroupBox(QVector<QWidget *> &widgets, QWidget *parent);
	static QAction *createMenuAction(
		const QString &iconName, const QString &text,
		QKeySequence::StandardKey shortcut = QKeySequence::UnknownKey);

	static QTreeWidgetItem *treeItem(QTreeWidget *tree,
		const QString &key, const QString &value);

	static auto treeItem(QTreeWidget *tree, const std::string &key,
		const std::string &value) -> QTreeWidgetItem *;

	static void openUrl(const QString &url, LinkType linkType, QWidget *parent);
	static void openUrl(const std::string &url, LinkType linkType, QWidget *parent);

	static bool darkBackground;

private:
	Utils() = default;
};
