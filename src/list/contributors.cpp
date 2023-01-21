#include "list/contributors.hpp"

List::Contributors::Contributors(const lib::http_client &httpClient)
	: httpClient(httpClient)
{
	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &List::Contributors::onItemDoubleClicked);
}

void List::Contributors::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	if (count() > 0)
	{
		return;
	}

	lib::gh::api(httpClient).contributors("kraxarn", "spotify-qt",
		[this](const std::vector<lib::gh::contributor> &contributors)
		{
			for (const auto &contributor: contributors)
			{
				auto *item = new QListWidgetItem(this);
				item->setText(QString::fromStdString(contributor.login));

				item->setToolTip(QString("%1 %2")
					.arg(contributor.contributions)
					.arg(contributor.contributions == 1
						? QStringLiteral("contribution")
						: QStringLiteral("contributions")));

				item->setData(urlRole,
					QString("https://github.com/kraxarn/spotify-qt/commits?author=%1")
						.arg(QString::fromStdString(contributor.login)));

				httpClient.get(contributor.avatar_url, lib::headers(),
					[item](const std::string &str)
					{
						auto data = QByteArray::fromStdString(str);
						QPixmap img;
						img.loadFromData(data);
						item->setIcon(QIcon(img));
					});
			}
		});
}

void List::Contributors::onItemDoubleClicked(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
	}

	const auto url = item->data(urlRole).toString();
	auto *parent = Parent::findWidget<Dialog::Settings>(parentWidget());
	if (parent != nullptr)
	{
		Url::open(url, LinkType::Web, parent);
	}
}
