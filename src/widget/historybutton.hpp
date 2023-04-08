#pragma once

#include "lib/spotify/playlist.hpp"
#include "lib/spotify/album.hpp"
#include "lib/spotify/show.hpp"
#include "util/icon.hpp"

#include <QAction>

class HistoryButton: public QAction
{
Q_OBJECT

public:
	explicit HistoryButton(QWidget *parent);

	void push(const lib::spt::playlist &playlist);
	void push(const lib::spt::album &album);
	void push(const lib::spt::show &show);

private:
	QAction *current = nullptr;

	void push(const lib::spt::entity &entity, const QVariant &data, const std::string &type);
	static auto getEntityId(QAction *action) -> std::string;

	void onTriggered(bool checked);
	void onMenuTriggered(QAction *action);
};