#pragma once

#include "lib/spotify/entity.hpp"
#include "util/icon.hpp"

#include <QAction>

class HistoryButton: public QAction
{
public:
	explicit HistoryButton(QWidget *parent);

	void push(const lib::spt::entity &entity);

private:
	QMenu *menu;
};
