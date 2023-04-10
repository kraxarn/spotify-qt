#include "widget/historybutton.hpp"

#include "util/font.hpp"
#include "mainwindow.hpp"
#include "lib/spotify/util.hpp"

#include <QMenu>

HistoryButton::HistoryButton(QWidget *parent)
	: QAction(parent)
{
	setIcon(Icon::get(QStringLiteral("go-previous")));
	setText(QStringLiteral("Go back"));
	setEnabled(false);
	setMenu(new QMenu());

	QAction::connect(this, &QAction::triggered,
		this, &HistoryButton::onTriggered);

	QMenu::connect(menu(), &QMenu::triggered,
		this, &HistoryButton::onMenuTriggered);
}

void HistoryButton::push(const lib::spt::playlist &playlist)
{
	push(playlist, QVariant::fromValue(playlist), "playlist");
}

void HistoryButton::push(const lib::spt::album &album)
{
	push(album, QVariant::fromValue(album), "album");
}

void HistoryButton::push(const lib::spt::show &show)
{
	push(show, QVariant::fromValue(show), "show");
}

void HistoryButton::push(const lib::spt::entity &entity,
	const QVariant &data, const std::string &type)
{
	if (current != nullptr && entity.id == getEntityId(current))
	{
		return;
	}

	auto *action = new QAction(QString::fromStdString(entity.name));
	action->setIcon(Icon::getByType(type));
	action->setData(data);

	const auto &actions = menu()->actions();
	if (actions.empty())
	{
		menu()->addAction(action);
	}
	else
	{
		auto *before = actions.first();
		menu()->insertAction(before, action);
	}

	setCurrent(action);
	setEnabled(true);
}

auto HistoryButton::getEntityId(QAction *action) -> std::string
{
	if (action->data().canConvert<lib::spt::playlist>())
	{
		return action->data().value<lib::spt::playlist>().id;
	}

	if (action->data().canConvert<lib::spt::album>())
	{
		return action->data().value<lib::spt::album>().id;
	}

	if (action->data().canConvert<lib::spt::show>())
	{
		return action->data().value<lib::spt::show>().id;
	}

	return {};
}

void HistoryButton::setCurrent(QAction *action)
{
	Font::setFontWeight(current, QFont::Normal);
	Font::setFontWeight(action, QFont::DemiBold);
	current = action;
}

void HistoryButton::onTriggered(bool /*checked*/)
{
	const auto &actions = menu()->actions();
	for (qsizetype i = 0; i < actions.size() - 1; i++)
	{
		if (actions.at(i) == current)
		{
			onMenuTriggered(actions.at(i + 1));
		}
	}
}

void HistoryButton::onMenuTriggered(QAction *action)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
	{
		return;
	}

	setCurrent(action);

	auto *tracksList = mainWindow->getSongsTree();
	if (action->data().canConvert<lib::spt::playlist>())
	{
		const auto playlist = action->data().value<lib::spt::playlist>();
		tracksList->load(playlist);
	}
	else if (action->data().canConvert<lib::spt::album>())
	{
		const auto album = action->data().value<lib::spt::album>();
		tracksList->load(album);
	}
	else if (action->data().canConvert<lib::spt::show>())
	{
		const auto show = action->data().value<lib::spt::show>();
		mainWindow->setSptContext(show);
	}
	else
	{
		lib::log::warn("Unknown type: {}", action->data().typeName());
	}
}
