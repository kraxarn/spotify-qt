#pragma once

#include <fstream>
#include <iostream>
#include <mutex>

// C++17 feature
#include <experimental/filesystem>

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
#include "enum/spotifycontext.hpp"
#include "lib/json.hpp"
#include "log.hpp"

using json = nlohmann::json;

namespace lib
{
	class Settings
	{
	public:
		Settings();

		json toJson() const;
		void fromJson(const json &json);
		void save();
		void load();

		typedef struct Account
		{
			std::string accessToken;
			std::string refreshToken;
			std::string clientId;
			std::string clientSecret;
		} Account;

		typedef struct General
		{
			Palette stylePalette = PaletteApp;
			PlaylistOrder playlistOrder = PlaylistOrderDefault;
			std::string lastPlaylist;
			std::string lastVersion;
			std::string style;
			std::vector<std::string> customPlaylistOrder;
			std::vector<int> hiddenSongHeaders;
			SpotifyContext trackNumbers = ContextAll;
			bool fallbackIcons = false;
			bool fixedWidthTime = true;
			bool mediaController = false;
			bool pulseVolume = false;
			bool showChangelog = true;
			bool showContextInfo = true;
			bool singleClickPlay = false;
			bool spotifyPlaybackOrder = true;
			bool trayAlbumArt = false;
			bool trayIcon = true;
			bool trayLightIcon = false;
			bool trayNotifications = false;
			int lastVolume = 0;
			int refreshInterval = 3;
			int songHeaderResizeMode = 3; // QHeaderView::ResizeToContents
			int songHeaderSortBy = -1;
		} General;

		typedef struct Spotify
		{
			std::string backend;
			std::string path;
			std::string username;
			bool alwaysStart = true;
			bool globalConfig = false;
			bool keyringPassword = false;
			bool startClient = false;
			int bitrate = 320;
			int maxQueue = 500;
		} Spotify;

		Account account;
		General general;
		Spotify spotify;

		void removeClient();
		void removeTokens();

		bool darkTheme() const;
		void setDarkTheme(bool value);

		static std::string fileName();
		static std::string filePath();

		static bool hasMediaControllerSupport();

		std::map<std::string, std::vector<std::string>> validate() const;

	private:
		std::mutex mutex;

		template<typename T>
		void setValue(const json &json, const std::string &key, T &value);
	};
}