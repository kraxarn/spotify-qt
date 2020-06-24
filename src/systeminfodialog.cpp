#include "systeminfodialog.hpp"

SystemInfoDialog::SystemInfoDialog(const QString &info, QWidget *parent) : QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("System info");

	auto textInfo = new QTextEdit(this);
	textInfo->setHtml(info);
	textInfo->setReadOnly(true);
	layout->addWidget(textInfo);

	auto infoAbout = new QLabel(
		"This information could be useful when reporting bugs. "
			"Additional information, depending on the type of issue, may be more helpful.", this);
	infoAbout->setWordWrap(true);
	layout->addWidget(infoAbout);

	auto buttons = new QDialogButtonBox();
	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok),
		&QPushButton::clicked, [this](bool checked) {
		accept();
	});
	layout->addWidget(buttons);
}