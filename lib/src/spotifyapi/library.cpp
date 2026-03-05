#include "lib/spotify/api.hpp"
#include "lib/spotify/saveditems.hpp"

void lib::spt::api::saved_albums(const ApiPagedCallback<saved_album> &callback) const
{
	request.get_page<saved_album>("me/albums", {}, callback);
}

void lib::spt::api::add_saved_albums(const std::vector<std::string> &album_ids,
	ApiCallback<std::string> &callback)
{
	put("me/albums", {
		{"ids", album_ids},
	}, callback);
}

void lib::spt::api::remove_saved_albums(const std::vector<std::string> &album_ids,
	ApiCallback<std::string> &callback)
{
	del("me/albums", {
		{"ids", album_ids},
	}, callback);
}

void lib::spt::api::is_saved_album(const std::vector<std::string> &album_ids,
	ApiCallback<std::vector<bool>> &callback)
{
	get(lib::fmt::format("me/albums/contains?ids={}",
		lib::strings::join(album_ids, ",")), callback);
}

void lib::spt::api::saved_tracks(ApiCallback<std::vector<lib::spt::track>> &callback)
{
	get_items("me/tracks?limit=50", callback);
}

void lib::spt::api::saved_tracks(const ApiPagedCallback<lib::spt::track> &callback)
{
	request.get_page<lib::spt::track>("me/tracks?limit=50", {}, callback);
}

void lib::spt::api::add_saved_tracks(const std::vector<std::string> &track_ids,
	ApiCallback<std::string> &callback)
{
	put("me/tracks", {
		{"ids", track_ids},
	}, callback);
}

void lib::spt::api::remove_saved_tracks(const std::vector<std::string> &track_ids,
	ApiCallback<std::string> &callback)
{
	del("me/tracks", {
		{"ids", track_ids},
	}, callback);
}

void lib::spt::api::isSavedItems(const QList<QString> &uris,
	ApiCallback<Result<SpotifySavedItems>> &callback) const
{
	const QString path = QStringLiteral("me/library/contains?uris=%1")
		.arg(uris.join(QChar::fromLatin1(',')));

	request.get(path, callback);
}

void lib::spt::api::saveItems(const QList<QString> &uris,
	ApiCallback<Result<Void>> &callback) const
{
	const QString path = QStringLiteral("me/library?uris=%1")
		.arg(uris.join(QChar::fromLatin1(',')));

	request.put(path, {}, callback);
}
