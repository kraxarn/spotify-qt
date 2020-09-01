#pragma once

#include "../spotify/clienthandler.hpp"

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>

class ClientHandlerLogDialog: public QDialog
{
Q_OBJECT

public:
	explicit ClientHandlerLogDialog(QWidget *parent = nullptr);
	int exec() override;

private:
	QTextEdit *logText = nullptr;

	void refresh();
	void scrollToBottom();
};