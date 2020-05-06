#include "texteditdialog.hpp"

TextEditDialog::TextEditDialog(const QString &title, const QString &label,
	const QString &text, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(title);
	setModal(true);
	auto layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(label));
	textEdit = new QTextEdit(text);
	layout->addWidget(textEdit);
	auto buttons = new QDialogButtonBox(this);
	buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	QDialogButtonBox::connect(buttons, &QDialogButtonBox::accepted, [this] {
		accept();
	});
	QDialogButtonBox::connect(buttons, &QDialogButtonBox::rejected, [this] {
		reject();
	});
	layout->addWidget(buttons);
	setLayout(layout);
}

QString TextEditDialog::text()
{
	return textEdit->toPlainText();
}

QString TextEditDialog::getText(const QString &title, const QString &label,
	const QString &text, QWidget *parent)
{
	auto dialog = new TextEditDialog(title, label, text, parent);
	if (dialog->exec() == DialogCode::Accepted)
		return dialog->text();
	return QString();
}
