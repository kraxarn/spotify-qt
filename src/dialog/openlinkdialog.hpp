#pragma once

#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class OpenLinkDialog : public QDialog
{
	Q_OBJECT

public:
	OpenLinkDialog(const QString &link, QWidget *parent = nullptr);
};