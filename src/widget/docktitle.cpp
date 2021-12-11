#include "widget/docktitle.hpp"

DockTitle::DockTitle(const QString &title, QDockWidget *parent)
	: QLabel(title, parent)
{
	parent->setTitleBarWidget(this);
	parent->setFloating(false);

	setContentsMargins(margins());
}

auto DockTitle::margins() -> QMargins
{
	return {
		marginHorizontal,
		marginVertical,
		marginHorizontal,
		marginVertical
	};
}
