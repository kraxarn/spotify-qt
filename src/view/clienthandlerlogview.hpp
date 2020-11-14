#pragma once

#include "../spotify/clienthandler.hpp"

#include <QClipboard>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidget>

class ClientHandlerLogView: public QTreeWidget
{
Q_OBJECT

public:
	explicit ClientHandlerLogView(QWidget *parent = nullptr);

private:
	void menu(const QPoint &pos);
};