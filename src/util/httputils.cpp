#include "httputils.hpp"

void HttpUtils::getAlbum(const std::string &url, const lib::http_client &httpClient,
	lib::cache &cache, lib::callback<QPixmap> &callback)
{
	if (url.empty())
	{
		callback(QPixmap());
		return;
	}

	auto data = cache.get_album_image(url);
	if (!data.empty())
	{
		QPixmap img;
		img.loadFromData(data.data(), data.size(), "jpeg");
		callback(img);
		return;
	}

	httpClient.get(url, lib::headers(),
		[&cache, url, callback](const std::string &str)
		{
			auto data = QByteArray::fromStdString(str);
			cache.set_album_image(url,
				std::vector<unsigned char>(data.begin(), data.end()));

			QPixmap img;
			img.loadFromData(data, "jpeg");
			callback(img);
		});

}
