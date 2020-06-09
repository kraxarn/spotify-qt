use std::io::{Read, Write};
use std::sync::{MutexGuard, PoisonError};
use tokio::io::Error;

enum Palette {
	App = 0,   // Default app palette
	Style = 1, // Palette from current style
	Dark = 2,  // Custom dark palette
}

#[derive(serde::Serialize, serde::Deserialize)]
pub struct Account {
	pub access_token: String,
	pub client_id: String,
	pub client_secret: String,
	pub refresh_token: String,
}

#[derive(serde::Serialize, serde::Deserialize)]
pub struct General {
	pub fallback_icons: bool,
	pub fixed_width_time: bool,
	pub hidden_song_headers: Vec<i32>,
	pub last_playlist: String,
	pub last_version: String,
	pub media_controller: bool,
	pub pulse_volume: bool,
	pub refresh_interval: i32,
	pub show_changelog: bool,
	pub song_header_resize_mode: i32,
	pub song_header_sort_by: i32,
	pub spotify_playback_order: bool,
	pub style: String,
	pub style_palette: u8,
	pub tray_icon: bool,
	pub tray_light_icon: bool,
	pub tray_notifications: bool,
}

#[derive(serde::Serialize, serde::Deserialize)]
pub struct Spotify {
	pub bitrate: i32,
	pub global_config: bool,
	pub path: String,
	pub start_client: bool,
	pub username: String,
}

#[derive(serde::Serialize, serde::Deserialize)]
pub struct Settings {
	pub account: Account,
	pub general: General,
	pub spotify: Spotify
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

	pub fn save(&self) -> std::result::Result<(), std::io::Error> {
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

pub struct SettingsManager {
	settings: std::sync::Mutex<Settings>
}

impl SettingsManager {
	pub fn new() -> Self {
		Self {
			settings: std::sync::Mutex::new(match Settings::load() {
				Ok(settings) => settings,
				Err(err) => {
					println!("warning: failed to load settings: {}", err);
					Settings::new()
				}
			})
		}
	}

	pub fn get(&self) -> std::sync::MutexGuard<Settings> {
		self.settings.lock()
			.expect("error: failed to get mutex lock for settings")
	}
}