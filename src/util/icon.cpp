#include "icon.hpp"

bool Icon::useFallbackIcons = false;

auto Icon::get(const QString &name) -> QIcon
{
	if (name.startsWith("logo:"))
	{
		return QIcon(QString(":/res/logo/%1.svg")
			.arg(name.right(name.length() - QString("logo:").length())));
	}

	auto fallback = QString(":/res/ic/%1/%2.svg")
		.arg(Style::getDarkBackground() ? "dark" : "light", name);

	return useFallbackIcons
		? QIcon(fallback)
		: QIcon::fromTheme(name, QIcon(fallback));
}

auto Icon::getByType(const std::string &type) -> QIcon
{
	if (type.empty())
	{
		return Icon::get(QStringLiteral("view-media-track"));
	}

	if (type == "album")
	{
		return Icon::get(QStringLiteral("view-media-album-cover"));
	}

	return Icon::get(QString("view-media-%1")
		.arg(QString::fromStdString(type)));
}
