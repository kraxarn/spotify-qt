#include "lib/curl/httpclient.hpp"

void lib::curl::http_client::get(const std::string &url, const lib::headers &headers,
	lib::callback<std::string> &callback) const
{
	cpr::GetCallback([callback](const cpr::Response &response)
	{
		callback(response.text);
	}, cpr::Url{url}, cpr_header(headers));
}

void lib::curl::http_client::put(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	cpr::PutCallback([callback](const cpr::Response &response)
	{
		callback(response.text);
	}, cpr::Url{url}, cpr_header(headers), cpr::Body{body});
}

void lib::curl::http_client::post(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	cpr::PostCallback([callback](const cpr::Response &response)
	{
		callback(response.text);
	}, cpr::Url(url), cpr_header(headers), cpr::Body{body});
}

auto lib::curl::http_client::post(const std::string &url, const lib::headers &headers,
	const std::string &post_data) const -> std::string
{
	return cpr::Post(cpr::Url{url}, cpr_header(headers), cpr::Body{post_data}).text;
}

void lib::curl::http_client::del(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	cpr::DeleteCallback([callback](const cpr::Response &response)
	{
		callback(response.text);
	}, cpr::Url{url}, cpr_header(headers), cpr::Body{body});
}

auto lib::curl::http_client::cpr_header(const lib::headers &headers) -> cpr::Header
{
	cpr::Header cpr_header;
	for (const auto &header : headers)
	{
		cpr_header[header.first] = header.second;
	}
	return cpr_header;
}
