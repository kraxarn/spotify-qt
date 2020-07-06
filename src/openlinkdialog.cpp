#include "openlinkdialog.hpp"

OpenLinkDialog::OpenLinkDialog(const QString &link, QWidget *parent) : QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Link error");

	auto title = new QLabel(
		"A default browser could not be found. "
		"Please try copying and pasting the following link in a web browser manually.", this);
	title->setWordWrap(true);
	layout->addWidget(title);

	auto textLink = new QTextEdit(link, this);
	textLink->setReadOnly(true);
	layout->addWidget(textLink);

	auto buttons = new QDialogButtonBox();
	QPushButton::connect(buttons->addButton("Copy to clipboard", QDialogButtonBox::ActionRole),
		&QPushButton::clicked, [link](bool checked) {
		QGuiApplication::clipboard()->setText(link);
	});

	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok),
		&QPushButton::clicked, [this](bool checked) {
		accept();
	});
	layout->addWidget(buttons);
}