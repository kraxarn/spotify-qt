#pragma once

#include "../spotify/clienthandler.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

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