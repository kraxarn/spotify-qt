use crate::spt::spotify::RequestError::{Network, Generic};
use tokio::io::Error;
use crate::spt::{Playlist, Device};
use super::reqwest::get;
use serde_json::Value;
use serde::Deserialize;

type Response = std::result::Result<serde_json::Value, reqwest::Error>;

enum RequestError {
	Generic(String),
	Io(std::io::Error),
	Network(reqwest::Error)
}

impl std::convert::From<reqwest::Error> for RequestError {
	fn from(err: reqwest::Error) -> Self {
		Network(err)
	}
}

#[derive(serde::Deserialize)]
struct RefreshResult {
	error_description: serde_json::Value,
	access_token: String
}

#[derive(serde::Deserialize)]
struct PlaylistsResult {
	items: Vec<Playlist>,
	total: i32
}

#[derive(serde::Deserialize)]
struct DevicesResult {
	devices: Vec<Device>
}

#[derive(serde::Deserialize)]
struct ErrorResult {
	error: Option<ErrorMessageResult>
}

#[derive(serde::Deserialize)]
struct ErrorMessageResult {
	message: String
}

impl From<ErrorResult> for Result<(), RequestError> {
	fn from(err: ErrorResult) -> Self {
		match err.error {
			Some(err) => Err(Generic(String::from(err.message))),
			None => Ok(()),
		}
	}
}

impl super::Spotify {
	async fn new(settings: crate::settings::SettingsManager) -> Self {
		let spt = Self {
			last_auth: 0,
			current_device: std::string::String::new(),
			settings,
			web_client: reqwest::Client::new()
		};
		//spt.refresh().await;
		spt
	}

	async fn get<D>(&self, url: &str) -> Result<D, reqwest::Error> where D: for<'de> Deserialize<'de> {
		self.web_client.get(url).send().await?.json::<D>().await
	}

	async fn put<D>(&self, url: &str, body: &[(&str, &str)]) -> Result<D, reqwest::Error>
		where D: for<'de> serde::Deserialize<'de> {
		self.web_client.put(url).json(body).send().await?.json::<D>().await
	}

	async fn post(&self, url: &str) -> Response {
		let mut headers = reqwest::header::HeaderMap::new();
		headers.insert(
			reqwest::header::CONTENT_TYPE,
			"application/x-www-form-urlencoded".parse().unwrap());
		self.web_client.post(url).headers(headers).send().await?.json().await
	}

	async fn delete<S>(&self, url: &str, body: &S) -> Response where S: serde::Serialize {
		let mut headers = reqwest::header::HeaderMap::new();
		headers.insert(
			reqwest::header::CONTENT_TYPE,
			"application/json".parse().unwrap());
		self.web_client.delete(url).headers(headers).json(body).send().await?.json().await
	}

	async fn refresh(&mut self) -> std::result::Result<(), RequestError> {
		// Make sure we have a refresh token
		let mut settings = self.settings.get();
		let s = &settings.account.refresh_token;
		if s.is_empty() {
			println!("warning: attempt to refresh without refresh token");
			return Err(RequestError::Generic(String::from("no refresh token")));
		}

		// Create request
		let json: RefreshResult = self.web_client.post("https://accounts.spotify.com/api/token")
			.header(reqwest::header::CONTENT_TYPE, "application/x-www-form-urlencoded")
			.header(reqwest::header::AUTHORIZATION,
				format!("Basic {}", base64::encode(
					format!("{}:{}", &settings.account.client_id, &settings.account.client_secret).as_bytes())))
			.form(&[
				("grant_type", "refresh_token"),
				("refresh_token", &settings.account.refresh_token)
			])
			.send().await?
			.json().await?;

		match json.error_description {
			serde_json::value::Value::String(error) =>
				Err(RequestError::Generic(format!("warning: failed to refresh token: {}", error))),
			_ => {
				self.last_auth = chrono::Utc::now().timestamp();
				settings.account.access_token = json.access_token;
				match settings.save() {
					Ok(_) => Ok(()),
					Err(err) => Err(RequestError::Io(err))
				}
			}
		}
	}
}