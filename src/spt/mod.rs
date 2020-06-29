extern crate reqwest;
extern crate chrono;

use serde::Deserialize;
use chrono::{DateTime, Utc, TimeZone};

pub mod spotify;

pub fn date_from_str(date: &str) -> DateTime<Utc> {
	Utc.datetime_from_str(date, "%Y-%m-%dT%H:%M:%SZ").unwrap()
}

pub struct Spotify {
	last_auth: i64,
	current_device: String,
	settings: super::settings::SettingsManager,
	web_client: reqwest::Client
}

pub struct SearchResults {
	albums: Vec<Album>,
	artists: Vec<Artist>,
	// For playlists, we need more complex information later
	playlists: Vec<serde_json::Value>,
	tracks: Vec<Track>
}

pub struct Album {
	album_group: String,
	artist: String,
	id: String,
	image: String,
	name: String,
	release_date: String //chrono::DateTime<chrono::Utc>
}

pub struct Artist {
	followers: i32,
	genres: Vec<String>,
	id: String,
	image: String,
	name: String,
	popularity: i32
}

#[derive(Deserialize)]
pub struct Track {
	added_at: String, //chrono::DateTime<chrono::Utc>,
	album: String,
	album_id: String,
	artist: String,
	artist_id: String,
	duration: i32,
	id: String,
	image: String,
	is_local: bool,
	name: String
}

#[derive(Deserialize)]
pub struct Playlist {
	collaborative: bool,
	description: String,
	id: String,
	image: String,
	is_public: bool,
	name: String,
	owner_id: String,
	owner_name: String,
	snapshot: String,
	tracks: serde_json::Value
}

#[derive(Deserialize)]
pub struct Device {
	id: String,
	name: String,
	is_active: bool
}