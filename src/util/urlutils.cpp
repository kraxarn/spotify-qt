#include "urlutils.hpp"

void UrlUtilities::open(const QString &url, LinkType linkType, QWidget *parent)
{
	if (!QDesktopServices::openUrl(QUrl(url)))
	{
		OpenLinkDialog(url, linkType, parent).exec();
	}
}

void UrlUtilities::open(const std::string &url, LinkType linkType, QWidget *parent)
{
	open(QString::fromStdString(url), linkType, parent);
}
