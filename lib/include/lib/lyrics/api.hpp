#pragma once

#include "lib/httpclient.hpp"
#include "lib/result.hpp"
#include "lib/lyrics/lyrics.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/spotify/track.hpp"

class LyricsApi
{
public:
	/**
	* Instance a new lyrics helper
	* @param httpClient HTTP client instance
	*/
	explicit LyricsApi(const HttpClient &httpClient);

	/**
	* Set app info for use in user-agent
	* @param name Application name
	* @param version Application version
	* @param homepage Application homepage
	*/
	void setAppInfo(const QString &name, const QString &version, const QString &homepage);

	/**
	* Search for lyrics
	* @param query Keyword of title, artist, or album
	* @param callback Lyrics
	*/
	void search(const QString &query, ApiCallback<Result<std::vector<lib::lrc::lyrics>>> &callback) const;

	/**
	* Fetch lyrics
	* @param track Track to fetch lyrics for
	* @param callback Lyrics
	*/
	void get(const lib::spt::track &track, ApiCallback<Result<lib::lrc::lyrics>> &callback) const;

	/**
	* Fetch lyrics by ID
	* @param lyricsId ID to fetch lyrics for
	* @param callback Lyrics
	*/
	void get(unsigned int lyricsId, ApiCallback<Result<lib::lrc::lyrics>> &callback) const;

private:
	const HttpClient &mHttp;
	QString mUserAgent;

	[[nodiscard]]
	auto headers() const -> RequestHeaders;
};
