#pragma once

#include "lib/result.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/callback.hpp"

#include <string>

#include <QNetworkAccessManager>

namespace lib
{
	using headers [[deprecated("Use RequestHeaders instead")]] = std::map<std::string, std::string>;
}

/**
* Request headers
*/
using Headers = QMap<QString, QString>;

class HttpClient final : public QObject
{
public:
	explicit HttpClient(QObject *parent);

	[[deprecated("Use with result callback instead")]]
	void get(const std::string &url, const lib::headers &headers,
		lib::callback<std::string> &callback) const;

	/**
	* GET request
	*/
	void get(const std::string &url, const lib::headers &headers,
		lib::callback<lib::result<std::string>> &callback) const;

	/**
	* PUT request with optional JSON body
	*/
	void put(const std::string &url, const std::string &body,
		const lib::headers &headers, lib::callback<std::string> &callback) const;

	[[deprecated("Use with result callback instead")]]
	void post(const std::string &url, const lib::headers &headers,
		lib::callback<std::string> &callback) const;

	/**
	* POST request without a request body
	*/
	void post(const std::string &url, const lib::headers &headers,
		lib::callback<lib::result<std::string>> &callback) const;

	[[deprecated("Use with result callback instead")]]
	void post(const std::string &url, const std::string &body,
		const lib::headers &headers, lib::callback<std::string> &callback) const;

	/**
	* POST request with a request body
	*/
	void post(const std::string &url, const std::string &body,
		const lib::headers &headers, lib::callback<lib::result<std::string>> &callback) const;

	[[nodiscard]]
	[[deprecated("Use asynchronous method instead")]]
	auto post(const std::string &url, const lib::headers &headers,
		const std::string &post_data) const -> std::string;

	/**
	* DELETE request with optional JSON body
	*/
	void del(const std::string &url, const std::string &body,
		const lib::headers &headers, lib::callback<std::string> &callback) const;

private:
	QNetworkAccessManager *mNetworkManager;

	static auto request(const std::string &url, const lib::headers &headers) -> QNetworkRequest;

	[[deprecated("Use with result callback instead")]]
	void await(QNetworkReply *reply, lib::callback<QByteArray> &callback) const;

	void await(QNetworkReply *reply, lib::callback<lib::result<std::string>> &callback) const;
};

namespace lib
{
	using http_client [[deprecated("Use HttpClient instead")]] = HttpClient;
}
