#include "dialog/icons.hpp"

#include <QVBoxLayout>
#include <QIcon>
#include <QMetaEnum>

Dialog::Icons::Icons(QWidget *parent)
	: Base(parent)
{
	auto *layout = Base::layout<QVBoxLayout>();

	list = new QListWidget(this);
	layout->addWidget(list);

	addAction(DialogAction::Ok);
}

void Dialog::Icons::showEvent(QShowEvent *event)
{
	Base::showEvent(event);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	for (auto i = static_cast<QIcon::ThemeIcon>(0);
		i < QIcon::ThemeIcon::NThemeIcons;
		i = static_cast<QIcon::ThemeIcon>(static_cast<int>(i) + 1))
	{
		const QIcon icon = QIcon::fromTheme(i);

		auto *item = new QListWidgetItem(list);

		if (icon.isNull())
		{
			item->setText(QStringLiteral("(missing icon: %1)")
				.arg(QString::number(static_cast<int>(i))));

			continue;
		}

		item->setText(icon.name());
		item->setIcon(icon);
	}
#endif
}
