#pragma once

#include "lib/result.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/callback.hpp"

#include <string>

#include <QNetworkAccessManager>

namespace lib
{
	/**
	 * @deprecated Use RequestHeaders instead
	 */
	using headers = std::map<std::string, std::string>;
}

/**
* Request headers
*/
// TODO: Change to QHttpHeaders when we drop Qt 5 support
using RequestHeaders = QMap<QString, QString>;

class HttpClient final : public QObject
{
	Q_OBJECT

public:
	explicit HttpClient(QObject *parent);

	[[deprecated("Use overload with result callback instead")]]
	void get(const std::string &url, const lib::headers &headers,
		ApiCallback<std::string> &callback) const;

	[[deprecated("Use overload with QUrl/QByteArray instead")]]
	void get(const std::string &url, const lib::headers &headers,
		ApiCallback<Result<std::string>> &callback) const;

	/**
	* GET request
	*/
	void get(const QUrl &url, const RequestHeaders &headers,
		const ApiCallback<Result<QByteArray>> &callback) const;

	/**
	* PUT request with optional JSON body
	*/
	void put(const std::string &url, const std::string &body,
		const lib::headers &headers, ApiCallback<std::string> &callback) const;

	[[deprecated("Use with result callback instead")]]
	void post(const std::string &url, const lib::headers &headers,
		ApiCallback<std::string> &callback) const;

	/**
	* POST request without a request body
	*/
	void post(const std::string &url, const lib::headers &headers,
		ApiCallback<Result<std::string>> &callback) const;

	[[deprecated("Use with result callback instead")]]
	void post(const std::string &url, const std::string &body,
		const lib::headers &headers, ApiCallback<std::string> &callback) const;

	/**
	* POST request with a request body
	*/
	void post(const std::string &url, const std::string &body,
		const lib::headers &headers, ApiCallback<Result<std::string>> &callback) const;

	[[nodiscard]]
	[[deprecated("Use asynchronous method instead")]]
	auto post(const std::string &url, const lib::headers &headers,
		const std::string &post_data) const -> std::string;

	/**
	* DELETE request with optional JSON body
	*/
	void del(const std::string &url, const std::string &body,
		const lib::headers &headers, ApiCallback<std::string> &callback) const;

private:
	QNetworkAccessManager *mNetworkManager;

	[[deprecated("Use overload with QUrl/RequestHeaders instead")]]
	static auto request(const std::string &url, const lib::headers &headers) -> QNetworkRequest;

	[[nodiscard]]
	static auto request(const QUrl &url, const RequestHeaders &headers) -> QNetworkRequest;

	[[deprecated("Use overload with result callback instead")]]
	void await(QNetworkReply *reply, ApiCallback<QByteArray> &callback) const;

	[[deprecated("Use overload with ApiCallback/QByteArray instead")]]
	void await(QNetworkReply *reply, ApiCallback<Result<std::string>> &callback) const;

	void await(QNetworkReply *reply, ApiCallback<Result<QByteArray>> &callback) const;
};
