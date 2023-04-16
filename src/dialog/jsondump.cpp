#include "dialog/jsondump.hpp"

Dialog::JsonDump::JsonDump(nlohmann::json json, QWidget *parent)
	: json(std::move(json)),
	Base(parent)
{
	resize(width, height);
	setWindowTitle(QStringLiteral("JSON"));
	auto *layout = Base::layout<QVBoxLayout>();

	textEdit = new QTextEdit(this);
	layout->addWidget(textEdit);

	Base::addAction(DialogAction::Ok);
}

void Dialog::JsonDump::showEvent(QShowEvent *event)
{
	textEdit->setPlainText(QString::fromStdString(json.dump(4)));
	QDialog::showEvent(event);
}
