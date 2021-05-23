#include "urlutils.hpp"

void UrlUtils::open(const QString &url, LinkType linkType, QWidget *parent)
{
	if (!QDesktopServices::openUrl(QUrl(url)))
	{
		OpenLinkDialog(url, linkType, parent).exec();
	}
}

void UrlUtils::open(const std::string &url, LinkType linkType, QWidget *parent)
{
	open(QString::fromStdString(url), linkType, parent);
}
