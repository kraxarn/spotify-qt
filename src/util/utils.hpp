#pragma once

#include "enum/linktype.hpp"
#include "enum/maskshape.hpp"
#include "lib/enum/palette.hpp"
#include "darkpalette.hpp"

#include <QAction>
#include <QApplication>
#include <QGroupBox>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>
#include <QPainterPath>
#include <QStringList>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QStyle>

class Utils
{
public:


	static auto mask(const QPixmap &source, MaskShape shape = MaskShape::App,
		const QVariant &data = QVariant()) -> QPixmap;

	static auto layoutToWidget(QLayout *layout) -> QWidget *;

	static void applyPalette(lib::palette palette);

	static auto createGroupBox(QVector<QWidget *> &widgets, QWidget *parent) -> QGroupBox *;

	static auto createMenuAction(const QString &iconName,
		const QString &text, QObject *parent,
		QKeySequence::StandardKey shortcut = QKeySequence::UnknownKey) -> QAction *;

	static void openUrl(const QString &url, LinkType linkType, QWidget *parent);
	static void openUrl(const std::string &url, LinkType linkType, QWidget *parent);

	static bool darkBackground;

private:
	Utils() = default;
};
