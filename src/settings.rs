extern crate serde;

use std::io::{Read, Write};

enum Palette {
	App = 0,   // Default app palette
	Style = 1, // Palette from current style
	Dark = 2,  // Custom dark palette
}

#[derive(serde::Serialize, serde::Deserialize)]
struct Account {
	access_token: String,
	client_id: String,
	client_secret: String,
	refresh_token: String,
}

#[derive(serde::Serialize, serde::Deserialize)]
struct General {
	fallback_icons: bool,
	fixed_width_time: bool,
	hidden_song_headers: Vec<i32>,
	last_playlist: String,
	last_version: String,
	media_controller: bool,
	pulse_volume: bool,
	refresh_interval: i32,
	show_changelog: bool,
	song_header_resize_mode: i32,
	song_header_sort_by: i32,
	spotify_playback_order: bool,
	style: String,
	style_palette: u8,
	tray_icon: bool,
	tray_light_icon: bool,
	tray_notifications: bool,
}

#[derive(serde::Serialize, serde::Deserialize)]
struct Spotify {
	bitrate: i32,
	global_config: bool,
	path: String,
	start_client: bool,
	username: String,
}

#[derive(serde::Serialize, serde::Deserialize)]
pub struct Settings {
	account: Account,
	general: General,
	spotify: Spotify
}

impl Account {
	fn new() -> Self {
		Self {
			access_token: std::string::String::new(),
			client_id: std::string::String::new(),
			client_secret: std::string::String::new(),
			refresh_token: std::string::String::new()
		}
	}
}

impl General {
	fn new() -> Self {
		Self {
			fallback_icons: false,
			fixed_width_time: true,
			hidden_song_headers: vec![],
			last_playlist: std::string::String::new(),
			last_version: std::string::String::new(),
			media_controller: false,
			pulse_volume: false,
			refresh_interval: 3,
			show_changelog: true,
			song_header_resize_mode: 3, // ResizeToContents
			song_header_sort_by: -1,
			spotify_playback_order: false,
			style: std::string::String::new(),
			style_palette: Palette::App.into(),
			tray_icon: false,
			tray_light_icon: false,
			tray_notifications: false
		}
	}
}

impl Spotify {
	fn new() -> Self {
		Self {
			bitrate: 320,
			global_config: false,
			path: std::string::String::new(),
			start_client: false,
			username: std::string::String::new()
		}
	}
}

impl std::convert::From<Palette> for u8 {
	fn from(p: Palette) -> Self {
		match p {
			Palette::App => 0,
			Palette::Style => 1,
			Palette::Dark => 2,
		}
	}
}

impl Settings {
	fn new() -> Self {
		Self {
			account: Account::new(),
			general: General::new(),
			spotify: Spotify::new()
		}
	}

	fn load() -> std::result::Result<Self, std::io::Error> {
		let mut settings = Settings::new();
		let mut file = std::fs::File::open(file_name())?;
		let mut data = String::new();
		file.read_to_string(&mut data)?;
		if data.is_empty() {
			println!("warning: json config in \"{:?}\" is empty", file_name().to_str());
			return Ok(settings);
		}
		settings = serde_json::from_str(&data)?;
		Ok(settings)
	}

	fn save(&self) -> std::result::Result<(), std::io::Error> {
		let data = serde_json::to_string(self)?;
		std::fs::write(file_name(), data)?;
		Ok(())
	}
}

fn file_path() -> std::path::PathBuf {
	match dirs_next::config_dir() {
		Some(dir) => dir.as_path().join("kraxarn"),
		None => std::path::PathBuf::new()
	}
}

fn file_name() -> std::path::PathBuf {
	file_path().join("spotify-qt.json")
}