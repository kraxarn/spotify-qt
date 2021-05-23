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


	static auto layoutToWidget(QLayout *layout) -> QWidget *;

	static void applyPalette(lib::palette palette);

	static auto createGroupBox(QVector<QWidget *> &widgets, QWidget *parent) -> QGroupBox *;

	static auto createMenuAction(const QString &iconName,
		const QString &text, QObject *parent,
		QKeySequence::StandardKey shortcut = QKeySequence::UnknownKey) -> QAction *;

	static bool darkBackground;

private:
	Utils() = default;
};
