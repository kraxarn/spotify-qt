#include "widget/historybutton.hpp"
#include "lib/developermode.hpp"
#include "lib/spotify/util.hpp"

#include <QMenu>

HistoryButton::HistoryButton(QWidget *parent)
	: QAction(parent)
{
	setIcon(Icon::get(QStringLiteral("go-previous")));
	setText(QStringLiteral("Go back"));
	setEnabled(false);
	setVisible(lib::developer_mode::enabled);
	setMenu(new QMenu());
}

void HistoryButton::push(const lib::spt::playlist &playlist)
{
	push(playlist, "playlist");
}

void HistoryButton::push(const lib::spt::album &album)
{
	push(album, "album");
}

void HistoryButton::push(const lib::spt::show &show)
{
	push(show, "show");
}

void HistoryButton::push(const lib::spt::entity &entity, const std::string &type)
{
	auto *action = new QAction(QString::fromStdString(entity.name));

	const auto uri = QString::fromStdString(lib::spt::id_to_uri(type, entity.id));
	action->setData(uri);

	const auto &actions = menu()->actions();

	if (actions.empty())
	{
		menu()->addAction(action);
	}
	else
	{
		auto *before = actions.first();
		const auto beforeUri = before->data().toString();
		if (uri != beforeUri)
		{
			menu()->insertAction(before, action);
		}
	}

	setEnabled(true);
}
