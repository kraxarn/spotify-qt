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

	QMenu::connect(menu(), &QMenu::triggered,
		this, &HistoryButton::onMenuTriggered);
}

void HistoryButton::push(const lib::spt::playlist &playlist)
{
	push(playlist.name, QVariant::fromValue(playlist), "playlist");
}

void HistoryButton::push(const lib::spt::album &album)
{
	push(album.name, QVariant::fromValue(album), "album");
}

void HistoryButton::push(const lib::spt::show &show)
{
	push(show.name, QVariant::fromValue(show), "show");
}

void HistoryButton::push(const std::string &name, const QVariant &entity, const std::string &type)
{
	auto *action = new QAction(QString::fromStdString(name));
	action->setIcon(Icon::getByType(type));
	action->setData(entity);
	Font::setFontWeight(action, QFont::DemiBold);

	const auto &actions = menu()->actions();
	if (actions.empty())
	{
		menu()->addAction(action);
	}
	else
	{
		auto *before = actions.first();
		if (getEntityId(action) == getEntityId(before))
		{
			return;
		}

		Font::setFontWeight(before, QFont::Normal);
		menu()->insertAction(before, action);
	}

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

void HistoryButton::onMenuTriggered(QAction *action)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
	{
		return;
	}

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
