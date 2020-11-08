#include "clienthandlerlogdialog.hpp"

ClientHandlerLogDialog::ClientHandlerLogDialog(QWidget *parent)
	: QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Spotify client log");

	logText = new QTextEdit(this);
	logText->setReadOnly(true);
	logText->setFont(QFont("monospace"));
	logText->setWordWrapMode(QTextOption::NoWrap);
	layout->addWidget(logText);

	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &ClientHandlerLogDialog::refresh);
	timer->start(1000);
	refresh();
}

int ClientHandlerLogDialog::exec()
{
	return spt::ClientHandler::getLog().isEmpty()
		? QMessageBox::information(parentWidget(), "No logs", "No logs found to display")
		: QDialog::exec();
}

void ClientHandlerLogDialog::refresh()
{
	logText->setText(spt::ClientHandler::getLog().join('\n'));
	scrollToBottom();
}

void ClientHandlerLogDialog::scrollToBottom()
{
	QTextCursor cursor = logText->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.movePosition(QTextCursor::StartOfLine);
	logText->setTextCursor(cursor);
}
