--region track

create table if not exists track
(
	id           text primary key not null,
	disc_number  integer          not null,
	duration_ms  integer          not null,
	explicit     integer          not null,
	href         text             not null,
	is_playable  integer          not null,
	name         text             not null,
	popularity   integer          not null,
	preview_url  text             not null,
	track_number integer          not null,
	type         text             not null,
	uri          text             not null,
	is_local     integer          not null
);

create table if not exists track_album
(
	track_id text not null,
	album_id text not null,
	foreign key (track_id) references track (id),
	foreign key (album_id) references album (id)
);

create table if not exists track_artist
(
	track_id  text not null,
	artist_id text not null,
	foreign key (track_id) references track (id),
	foreign key (artist_id) references artist (id)
);

create table if not exists track_market
(
	track_id text not null,
	country  text not null,
	foreign key (track_id) references track (id)
);

create table if not exists track_external_id
(
	track_id text not null,
	isrc     text not null,
	ean      text not null,
	upx      text not null,
	foreign key (track_id) references track (id)
);

create table if not exists track_external_url
(
	track_id text not null,
	spotify  text not null,
	foreign key (track_id) references track (id)
);

create table if not exists track_restriction
(
	track_id text not null,
	reason   text not null,
	foreign key (track_id) references track (id)
);

--region track_link

create table if not exists track_link
(
	id   text primary key not null,
	href text             not null,
	type text             not null,
	uri  text             not null
);

create table if not exists track_link_external_url
(
	track_link_id text not null,
	spotify       text not null
);

--endregion

--region album

create table if not exists album
(
	id                     text primary key not null,
	album_type             text             not null,
	total_tracks           integer          not null,
	href                   text             not null,
	name                   text             not null,
	release_date           text             not null,
	release_date_precision text             not null,
	uri                    text             not null
);

create table if not exists album_market
(
	album_id text not null,
	country  text not null,
	foreign key (album_id) references album (id)
);

create table if not exists album_external_url
(
	album_id    text not null,
	url         text not null,
	description text not null,
	foreign key (album_id) references album (id)
);

create table if not exists album_image
(
	album_id text    not null,
	url      text    not null,
	width    integer not null,
	height   integer not null,
	data     blob    null,
	foreign key (album_id) references album (id)
);

create table if not exists album_restriction
(
	album_id text not null,
	reason   text not null,
	foreign key (album_id) references album (id)
);

create table if not exists album_artist
(
	album_id  text not null,
	artist_id text not null,
	foreign key (album_id) references album (id),
	foreign key (artist_id) references artist (id)
);

create table if not exists album_track
(
	album_id text not null,
	track_id text not null,
	foreign key (album_id) references album (id),
	foreign key (track_id) references track (id)
);

--endregion

--region artist

create table if not exists artist
(
	id         text primary key not null,
	name       text             not null,
	href       text             not null,
	uri        text             not null,
	followers  integer          not null,
	popularity integer          not null
);

create table if not exists artist_external_url
(
	artist_id   text not null,
	url         text not null,
	description text not null,
	foreign key (artist_id) references artist (id)
);

create table if not exists artist_genre
(
	artist_id text not null,
	genre     text not null,
	foreign key (artist_id) references artist (id)
);

create table if not exists artist_image
(
	artist_id text    not null,
	url       text    not null,
	width     integer not null,
	height    integer not null,
	data      blob    null,
	foreign key (artist_id) references artist (id)
);

--endregion

--region show

create table if not exists show
(
	id                   text primary key not null,
	description          text             not null,
	html_description     text             not null,
	explicit             boolean          not null,
	href                 text             not null,
	is_externally_hosted boolean          not null,
	media_type           text             not null,
	name                 text             not null,
	publisher            text             not null,
	uri                  text             not null
);

create table if not exists show_market
(
	show_id text not null,
	country text not null,
	foreign key (show_id) references show (id)
);

create table if not exists show_copyright
(
	show_id text not null,
	text    text not null,
	type    text not null,
	foreign key (show_id) references show (id)
);

create table if not exists show_external_url
(
	show_id     text not null,
	url         text not null,
	description text not null,
	foreign key (show_id) references show (id)
);

create table if not exists show_image
(
	show_id text    not null,
	url     text    not null,
	height  integer not null,
	width   integer not null,
	foreign key (show_id) references show (id)
);

create table if not exists show_language
(
	show_id text not null,
	code    text not null,
	foreign key (show_id) references show (id)
);

create table if not exists show_episode
(
	show_id    text not null,
	episode_id text not null,
	foreign key (show_id) references show (id),
	foreign key (episode_id) references episode (id)
);

--endregion

--region episode

create table if not exists episode
(
	id                     text primary key not null,
	audio_preview_url      text             not null,
	description            text             not null,
	html_description       text             not null,
	duration_ms            integer          not null,
	explicit               boolean          not null,
	href                   text             not null,
	is_externally_hosted   integer          not null,
	is_playable            integer          not null,
	name                   text             not null,
	release_date           text             not null,
	release_date_precision text             not null,
	url                    text             not null
);

create table if not exists episode_external_url
(
	episode_id  text not null,
	url         text not null,
	description text not null,
	foreign key (episode_id) references episode (id)
);

create table if not exists episode_image
(
	episode_id text    not null,
	url        text    not null,
	height     integer not null,
	width      integer not null,
	foreign key (episode_id) references episode (id)
);

create table if not exists episode_langauge
(
	episode_id text not null,
	code       text not null,
	foreign key (episode_id) references episode (id)
);

create table if not exists episode_resume_point
(
	episode_id         text    not null,
	fully_played       integer not null,
	resume_position_ms integer not null,
	foreign key (episode_id) references episode (id)
);

create table if not exists episode_restriction
(
	episode_id text not null,
	reason     text not null,
	foreign key (episode_id) references episode (id)
);

--endregion