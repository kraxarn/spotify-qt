#pragma once

#include "lib/enum/albumgroup.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "view/artist/albumlist.hpp"
#include "util/dateutils.hpp"
#include "util/httputils.hpp"
#include "enum/datarole.hpp"

#include <QTabWidget>
#include <QHash>
#include <QDateTime>

namespace View
{
	namespace Artist
	{
		class Albums: public QTabWidget
		{
		public:
			Albums(lib::spt::api &spotify, lib::cache &cache,
				const lib::http_client &httpClient, QWidget *parent);

			void setAlbums(const std::vector<lib::spt::album> &albums);

		private:
			lib::cache &cache;
			const lib::http_client &httpClient;

			std::map<lib::album_group, View::Artist::AlbumList *> lists;

			static auto toString(lib::album_group albumGroup) -> QString;
		};
	}
}
