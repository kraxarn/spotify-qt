#include "openlinkdialog.hpp"

OpenLinkDialog::OpenLinkDialog(const QString &link, LinkType linkType, QWidget *parent)
	: linkType(linkType), QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Link error");

	auto title = new QLabel(titleText(), this);
	title->setWordWrap(true);
	layout->addWidget(title);

	auto textLink = new QTextEdit(link, this);
	textLink->setReadOnly(true);
	layout->addWidget(textLink);

	auto buttons = new QDialogButtonBox(this);
	QPushButton::connect(buttons->addButton("Copy to clipboard", QDialogButtonBox::ActionRole),
		&QPushButton::clicked, [link](bool checked)
		{
			QGuiApplication::clipboard()->setText(link);
		});

	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok), &QPushButton::clicked,
		[this](bool checked)
		{
			accept();
		});

	layout->addWidget(buttons);
}

QString OpenLinkDialog::titleText()
{
	return QString(
		"A default %1 browser could not be found. "
		"Please try copying and pasting the following link in a %1 browser manually.")
		.arg(linkType == LinkType::Web ? "web" : "file");
}
