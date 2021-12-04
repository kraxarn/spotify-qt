#pragma once

#include "spotifyclient/runner.hpp"

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidget>

class ClientHandlerLogView: public QTreeWidget
{
Q_OBJECT

public:
	explicit ClientHandlerLogView(QWidget *parent = nullptr);

protected:
	void showEvent(QShowEvent *event) override;

private:
	void menu(const QPoint &pos);
};
