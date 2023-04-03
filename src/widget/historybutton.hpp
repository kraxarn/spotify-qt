#pragma once

#include "lib/spotify/entity.hpp"
#include "util/icon.hpp"

#include <QAction>

class HistoryButton: public QAction
{
public:
	explicit HistoryButton(QWidget *parent);
	~HistoryButton() override;

	template<typename T>
	void push(const T &entity)
	{
		auto *pointer = new T(entity);
		items.append(pointer);
	}

private:
	QList<lib::spt::entity *> items;
};
