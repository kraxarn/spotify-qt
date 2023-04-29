#include "dialog/jsondump.hpp"
#include "util/font.hpp"

Dialog::JsonDump::JsonDump(nlohmann::json json, QWidget *parent)
	: Base(parent),
	json(std::move(json))
{
	resize(width, height);
	setWindowTitle(QStringLiteral("JSON"));
	auto *layout = Base::layout<QVBoxLayout>();

	textEdit = new QTextEdit(this);
	textEdit->setWordWrapMode(QTextOption::NoWrap);
	textEdit->setFont(Font::monospace());
	layout->addWidget(textEdit);

	Base::addAction(DialogAction::Ok);
}

void Dialog::JsonDump::showEvent(QShowEvent *event)
{
	textEdit->setPlainText(QString::fromStdString(json.dump(4)));
	QDialog::showEvent(event);
}
