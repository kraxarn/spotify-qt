#include "widget/historybutton.hpp"
#include "lib/developermode.hpp"

HistoryButton::HistoryButton(QWidget *parent)
	: QAction(parent)
{
	setIcon(Icon::get(QStringLiteral("go-previous")));
	setText(QStringLiteral("Go back"));
	setEnabled(false);
	setVisible(lib::developer_mode::enabled);
}
