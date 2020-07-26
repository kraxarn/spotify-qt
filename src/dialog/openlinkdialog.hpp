#pragma once

#include "../enum/linktype.hpp"
#include "../util/utils.hpp"

#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class OpenLinkDialog: public QDialog
{
Q_OBJECT

public:
	OpenLinkDialog(const QString &link, LinkType linkType, QWidget *parent = nullptr);

private:
	LinkType linkType;

	QString titleText();
};