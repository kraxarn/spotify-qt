use crate::spt::spotify::RequestError::{Network, Generic};
use tokio::io::Error;
use crate::spt::{Playlist, Device, Track};
use super::reqwest::{get, Method, RequestBuilder};
use serde_json::{json, Value};
use serde::{Deserialize, Serialize};
use serde::export::fmt::Display;
use serde::export::Formatter;

#[derive(Debug)]
enum RequestError {
	Generic(String),
	Io(std::io::Error),
	Network(reqwest::Error)
}

type Response = std::result::Result<serde_json::Value, reqwest::Error>;
type RequestResult<T> = Result<T, RequestError>;

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
		let mut spt = Self {
			last_auth: 0,
			current_device: std::string::String::new(),
			settings,
			web_client: reqwest::Client::new()
		};
		spt.refresh().await.unwrap();
		spt
	}

	async fn request(&mut self, method: Method, url: &str) -> RequestBuilder {
		let last_refresh = chrono::Utc::now().timestamp() - self.last_auth;
		if last_refresh >= 36000 {
			println!("access token probably expired, refreshing");
			match self.refresh().await {
				Ok(()) => {},
				Err(err) => println!("error: failed to refresh token: {:?}", err),
			}
		}

		let mut headers = reqwest::header::HeaderMap::new();
		headers.insert(
			reqwest::header::AUTHORIZATION,
			format!("Bearer {}", self.settings.get().account.access_token).parse().unwrap());
		self.web_client
			.request(method, url)
			.headers(headers)
	}

	async fn get<D>(&mut self, url: &str) -> Result<D, reqwest::Error>
		where D: for<'de> Deserialize<'de> {
		self.request(Method::GET, url).await
			.send().await?
			.json::<D>().await
	}

	async fn put<D>(&mut self, url: &str, body: &serde_json::Value) -> Result<D, reqwest::Error>
		where D: for<'de> serde::Deserialize<'de> {
		self.request(Method::PUT, url).await
			.json(body)
			.send().await?
			.json::<D>().await
	}

	async fn post<D>(&mut self, url: &str) -> Result<D, reqwest::Error>
		where D: for<'de> serde::Deserialize<'de> {
		let mut headers = reqwest::header::HeaderMap::new();
		headers.insert(
			reqwest::header::CONTENT_TYPE,
			"application/x-www-form-urlencoded".parse().unwrap());
		self.request(Method::POST, url).await
			.headers(headers)
			.send().await?
			.json().await
	}

	async fn delete<S>(&mut self, url: &str, body: &S) -> Response where S: serde::Serialize {
		let mut headers = reqwest::header::HeaderMap::new();
		headers.insert(
			reqwest::header::CONTENT_TYPE,
			"application/json".parse().unwrap());
		self.request(Method::DELETE, url).await
			.headers(headers)
			.json(body)
			.send().await?
			.json().await
	}

	async fn refresh(&mut self) -> RequestResult<()> {
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