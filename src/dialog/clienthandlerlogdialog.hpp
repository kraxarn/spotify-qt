#pragma once

#include "../spotify/clienthandler.hpp"

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollBar>

class ClientHandlerLogDialog: public QDialog
{
Q_OBJECT

public:
	explicit ClientHandlerLogDialog(QWidget *parent = nullptr);
	int exec() override;
};