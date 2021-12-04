#pragma once

#include "enum/linktype.hpp"

#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace Dialog
{
	class OpenLink: public QDialog
	{
	Q_OBJECT

	public:
		OpenLink(const QString &link, LinkType linkType, QWidget *parent);

	private:
		LinkType linkType;

		auto titleText() -> QString;
	};
}
