#include "widget/historybutton.hpp"
#include "lib/developermode.hpp"

#include <QMenu>

HistoryButton::HistoryButton(QWidget *parent)
	: QAction(parent)
{
	setIcon(Icon::get(QStringLiteral("go-previous")));
	setText(QStringLiteral("Go back"));
	setEnabled(false);
	setVisible(lib::developer_mode::enabled);

	menu = new QMenu();
	setMenu(menu);
}

void HistoryButton::push(const lib::spt::entity &entity)
{
	menu->addAction(QString::fromStdString(entity.name));
	setEnabled(true);
}
