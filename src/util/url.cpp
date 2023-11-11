#include "util/url.hpp"

void Url::open(const QString &url, LinkType linkType, QWidget *parent)
{
	QUrl qUrl(url);
	if (qUrl.scheme().isEmpty())
	{
		qUrl.setScheme(getDefaultScheme(linkType));
	}

	if (!QDesktopServices::openUrl(qUrl))
	{
		Dialog::OpenLink(url, linkType, parent).exec();
	}
}

void Url::open(const std::string &url, LinkType linkType, QWidget *parent)
{
	open(QString::fromStdString(url), linkType, parent);
}

auto Url::getDefaultScheme(const LinkType linkType) -> QString
{
	switch (linkType)
	{
		case LinkType::Web:
			return QStringLiteral("https");

		case LinkType::Path:
			return QStringLiteral("file");

		default:
			return {};
	}
}
