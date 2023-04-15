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

void HistoryButton::push(const lib::spt::entity &entity)
{
	push(entity, QVariant::fromValue(entity), "track");
}

void HistoryButton::push(const lib::spt::entity &entity,
	const QVariant &data, const std::string &type)
{
	if (current != nullptr && entity.id == getEntityId(current))
	{
		return;
	}

	for (auto *menuAction: menu()->actions())
	{
		if (menuAction == current)
		{
			break;
		}

		menu()->removeAction(menuAction);
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

void HistoryButton::back()
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

void HistoryButton::forward()
{
	const auto &actions = menu()->actions();
	for (qsizetype i = 0; i < actions.size() - 1; i++)
	{
		if (actions.at(i + 1) == current)
		{
			onMenuTriggered(actions.at(i));
		}
	}
}

auto HistoryButton::currentUri() const -> std::string
{
	const auto &data = current->data();

	if (data.canConvert<lib::spt::playlist>())
	{
		const auto playlistId = data.value<lib::spt::playlist>().id;
		return lib::spt::id_to_uri("playlist", playlistId);
	}

	if (data.canConvert<lib::spt::album>())
	{
		const auto albumId = data.value<lib::spt::album>().id;
		return lib::spt::id_to_uri("album", albumId);
	}

	if (data.canConvert<lib::spt::show>())
	{
		const auto showId = data.value<lib::spt::show>().id;
		return lib::spt::id_to_uri("show", showId);
	}

	return {};
}

auto HistoryButton::getEntityId(QAction *action) -> std::string
{
	const auto &data = action->data();

	if (data.canConvert<lib::spt::playlist>())
	{
		return data.value<lib::spt::playlist>().id;
	}

	if (data.canConvert<lib::spt::album>())
	{
		return data.value<lib::spt::album>().id;
	}

	if (data.canConvert<lib::spt::show>())
	{
		return data.value<lib::spt::show>().id;
	}

	if (data.canConvert<lib::spt::entity>())
	{
		return data.value<lib::spt::entity>().id;
	}

	return {};
}

void HistoryButton::setCurrent(QAction *action)
{
	if (current != nullptr)
	{
		Font::setFontWeight(current, QFont::Normal);
	}

	Font::setFontWeight(action, QFont::DemiBold);
	current = action;
}

auto HistoryButton::load(const QVariant &data) -> bool
{
	auto *mainWindow = MainWindow::find(parent());
	if (mainWindow == nullptr)
	{
		return false;
	}

	if (data.canConvert<lib::spt::playlist>())
	{
		const auto playlist = data.value<lib::spt::playlist>();
		mainWindow->getSongsTree()->load(playlist);
		return true;
	}

	if (data.canConvert<lib::spt::album>())
	{
		const auto album = data.value<lib::spt::album>();
		mainWindow->getSongsTree()->load(album);
		return true;
	}

	if (data.canConvert<lib::spt::show>())
	{
		lib::log::error("Shows currently not supported");
		return false;
	}

	if (data.canConvert<lib::spt::entity>())
	{
		auto *library = mainWindow->findChild<List::Library *>();
		if (library == nullptr)
		{
			return false;
		}

		const auto entity = data.value<lib::spt::entity>();
		for (int i = 0; i < library->topLevelItemCount(); i++)
		{
			auto *item = library->topLevelItem(i);
			if (item->text(0) == QString::fromStdString(entity.name))
			{
				library->setCurrentItem(item);
				library->load(item);
				return true;
			}
		}

		return false;
	}

	lib::log::warn("Unknown type: {}", data.typeName());
	return false;
}

void HistoryButton::onTriggered(bool /*checked*/)
{
	back();
}

void HistoryButton::onMenuTriggered(QAction *action)
{
	if (load(action->data()))
	{
		setCurrent(action);
	}
}
