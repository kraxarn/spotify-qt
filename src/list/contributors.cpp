#include "list/contributors.hpp"

List::Contributors::Contributors(const lib::http_client &httpClient)
	: httpClient(httpClient)
{
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
