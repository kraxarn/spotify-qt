#include "clienthandlerlogdialog.hpp"

ClientHandlerLogDialog::ClientHandlerLogDialog(QWidget *parent)
	: QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Spotify client log");
	setMinimumSize(600, 400);

	auto logText = new QTextEdit(this);
	logText->setReadOnly(true);
	logText->setFont(QFont("monospace"));
	logText->setWordWrapMode(QTextOption::NoWrap);
	logText->setText(spt::ClientHandler::getLog().join('\n'));
	layout->addWidget(logText);

	QTextCursor cursor = logText->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.movePosition(QTextCursor::StartOfLine);
	logText->setTextCursor(cursor);

	auto buttons = new QDialogButtonBox();
	QPushButton::connect(
		buttons->addButton(QDialogButtonBox::Ok), &QPushButton::clicked,
		[this](bool checked)
		{
			accept();
		});

	layout->addWidget(buttons);
}

int ClientHandlerLogDialog::exec()
{
	return
		spt::ClientHandler::getLog().isEmpty()
		? QMessageBox::information(parentWidget(), "No logs", "No logs found to display")
		: QDialog::exec();
}
