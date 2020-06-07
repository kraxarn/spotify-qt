type Response = std::result::Result<serde_json::Value, reqwest::Error>;

impl super::Spotify {
	async fn new(settings: crate::settings::SettingsManager) -> Self {
		let spt = Self {
			last_auth: 0,
			current_device: std::string::String::new(),
			settings,
			web_client: reqwest::Client::new()
		};
		spt.refresh().await;
		spt
	}

	async fn get(&self, url: &str) -> Response {
		self.web_client.get(url).send().await?.json().await
	}

	async fn put<S>(&self, url: &str, body: &S) -> Response where S: serde::Serialize {
		self.web_client.put(url).json(body).send().await?.json().await
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

	async fn refresh(&self) {
	}
}