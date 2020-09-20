#pragma once

#include "../enum/linktype.hpp"
#include "../enum/maskshape.hpp"
#include "../enum/palette.hpp"
#include "darkpalette.hpp"

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

class Utils
{

public:
	static QJsonValue getProperty(const QJsonObject &json, const QStringList &names);
	static QTreeWidgetItem *treeItemWithChildren(QTreeWidget *tree, const QString &name,
		const QString &toolTip = QString::Null(),
		const QStringList &childrenItems = QStringList(QString::Null()));
	static QPixmap mask(const QPixmap &source, MaskShape shape = MaskShape::App,
		const QVariant &data = QVariant());
	static QWidget *layoutToWidget(QLayout *layout);
	static void applyPalette(Palette palette);
	static QString formatTime(int ms);
	static QGroupBox *createGroupBox(QVector<QWidget *> &widgets, QWidget *parent);
	static QAction *createMenuAction(
		const QString &iconName, const QString &text,
		QKeySequence::StandardKey shortcut = QKeySequence::UnknownKey);
	static QTreeWidgetItem *treeItem(
		QTreeWidget *tree, const QString &key, const QString &value);
	static void openUrl(const QString &url, LinkType linkType, QWidget *parent);

	static bool darkBackground;

private:
	Utils() = default;
};
