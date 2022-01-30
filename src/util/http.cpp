#include "util/http.hpp"

void Http::getAlbum(const std::string &url, const lib::http_client &httpClient,
	lib::cache &cache, bool useDefaultIcon, lib::callback<QPixmap> &callback)
{
	if (url.empty())
	{
		callback(QPixmap());
		return;
	}

	auto data = cache.get_album_image(url);
	if (lib::image::is_jpeg(data))
	{
		QPixmap img;
		img.loadFromData(data.data(), static_cast<unsigned int>(data.size()), "jpeg");
		callback(img);
		return;
	}

	if (useDefaultIcon)
	{
		callback(defaultIcon());
	}

	httpClient.get(url, lib::headers(),
		[&cache, url, callback](const std::string &str)
		{
			auto data = QByteArray::fromStdString(str);
			if (!lib::image::is_jpeg(std::vector<unsigned char>(data.begin(), data.end())))
			{
				lib::log::warn("Album art from \"{}\" is not a valid JPEG image",
					url);
				return;
			}
			cache.set_album_image(url,
				std::vector<unsigned char>(data.begin(), data.end()));

			QPixmap img;
			img.loadFromData(data, "jpeg");
			callback(img);
		});
}

void Http::getAlbum(const std::string &url, const lib::http_client &httpClient,
	lib::cache &cache, lib::callback<QPixmap> &callback)
{
	getAlbum(url, httpClient, cache, true, callback);
}

auto Http::defaultIcon() -> QPixmap
{
	constexpr int iconSize = 64;
	return Icon::get("media-optical-audio").pixmap(iconSize);
}
