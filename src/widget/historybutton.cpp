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
	action->setIcon(Icon::getByType(type));
	Font::setFontWeight(action, QFont::DemiBold);

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
		if (uri == beforeUri)
		{
			return;
		}

		Font::setFontWeight(before, QFont::Normal);
		menu()->insertAction(before, action);
	}

	setEnabled(true);
}

void HistoryButton::onMenuTriggered(QAction *action)
{
	const auto uri = action->data().toString();
	const auto id = lib::spt::uri_to_id(uri.toStdString());

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
	{
		return;
	}

	auto *tracksList = mainWindow->getSongsTree();
	if (uri.startsWith(QStringLiteral("spotify:playlist:")))
	{
		lib::spt::playlist playlist;
		playlist.id = id;
		tracksList->load(playlist);
	}
	else if (uri.startsWith(QStringLiteral("spotify:album:")))
	{
		lib::spt::album album;
		album.id = id;
		tracksList->load(album);
	}
	else if (uri.startsWith(QStringLiteral("spotify:show:")))
	{
		lib::spt::show show;
		show.id = id;
		mainWindow->setSptContext(show);
	}
	else
	{
		lib::log::warn("Unknown URI: {}", uri.toStdString());
	}
}
