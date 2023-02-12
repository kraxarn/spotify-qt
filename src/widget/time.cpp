#include "widget/time.hpp"
#include "util/font.hpp"

#include "lib/time.hpp"
#include "lib/log.hpp"

#include <QHBoxLayout>

Time::Time(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);
	layout->setContentsMargins({});
	layout->setSpacing(0);

	const auto width = Font::numberWidth();
	for (auto i = 0; i < labelCount; i++)
	{
		auto *label = new QLabel(this);
		label->setAlignment(label->alignment() | Qt::AlignHCenter);

		if (i == 2)
		{
			label->setText(QStringLiteral(":"));
		}
		else
		{
			label->setText(QStringLiteral("0"));
			label->setFixedWidth(width);
		}
		layout->addWidget(label);
	}
}

void Time::setTime(int milliseconds)
{
	const auto minutes = milliseconds / lib::time::ms_in_min;
	const auto seconds = (milliseconds / lib::time::ms_in_sec) % 60;

	auto *minuteHigh = labelAt(0);
	minuteHigh->setText(QString::number(minutes / 10));
	minuteHigh->setVisible(minutes >= 10);

	labelAt(1)->setText(QString::number(minutes % 10));
	labelAt(3)->setText(QString::number(seconds / 10));
	labelAt(4)->setText(QString::number(seconds % 10));
}

auto Time::labelAt(int index) -> QLabel *
{
	return qobject_cast<QLabel *>(layout()->itemAt(index)->widget());
}
