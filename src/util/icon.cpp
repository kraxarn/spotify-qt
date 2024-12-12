#include "icon.hpp"

bool Icon::useFallbackIcons = false;

auto Icon::get(const QString &name) -> QIcon
{
	if (name.startsWith("logo:"))
	{
		return QIcon(QStringLiteral(":/res/logo/%1.svg")
			.arg(name.right(name.length() - QStringLiteral("logo:").length())));
	}

	auto fallback = QStringLiteral(":/res/ic/%1/%2.svg")
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

	return Icon::get(QStringLiteral("view-media-%1")
		.arg(QString::fromStdString(type)));
}
