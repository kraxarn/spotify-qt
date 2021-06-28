#include "widget/docktitle.hpp"

Widget::DockTitle::DockTitle(const QString &title, QDockWidget *parent)
	: QLabel(title, parent)
{
	parent->setTitleBarWidget(this);
	parent->setFloating(false);

	setContentsMargins(margins());
}

auto Widget::DockTitle::margins() -> QMargins
{
	return {
		marginHorizontal,
		marginVertical,
		marginHorizontal,
		marginVertical
	};
}
