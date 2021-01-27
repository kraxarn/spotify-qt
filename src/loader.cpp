#include "loader.hpp"

Loader::Loader(QLabel *parent)
	: label(parent),
	QObject(parent)
{
	labelText = label->text();
	limit = label->text().length();

	timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &Loader::timeout);
	timer->start(800);
}

void Loader::timeout()
{
	if (label == nullptr || label->text() != labelText)
	{
		timer->stop();
		deleteLater();
		return;
	}

	labelText = labelText.length() >= limit
		? QString()
		: QString("%1.").arg(label->text());
	label->setText(labelText);
}
