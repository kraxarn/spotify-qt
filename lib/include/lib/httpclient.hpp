#pragma once

#include "lib/settings.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/result.hpp"

#include <string>

#include <QNetworkAccessManager>

namespace lib
{
	/**
	 * Request headers
	 */
	using headers = std::map<std::string, std::string>;

	class http_client final : public QObject
	{
	public:
		explicit http_client(QObject *parent);

		[[deprecated("Use with result callback instead")]]
		void get(const std::string &url, const headers &headers,
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
			const headers &headers, lib::callback<std::string> &callback) const;

		[[deprecated("Use with result callback instead")]]
		void post(const std::string &url, const headers &headers,
			lib::callback<std::string> &callback) const;

		/**
		 * POST request without a request body
		 */
		void post(const std::string &url, const headers &headers,
			lib::callback<lib::result<std::string>> &callback) const;

		[[deprecated("Use with result callback instead")]]
		void post(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const;

		/**
		 * POST request with a request body
		 */
		void post(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<lib::result<std::string>> &callback) const;

		[[deprecated("Use asynchronous method instead")]]
		auto post(const std::string &url, const headers &headers,
			const std::string &post_data) const -> std::string;

		/**
		 * DELETE request with optional JSON body
		 */
		void del(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const;

	private:
		QNetworkAccessManager *mNetworkManager;

		static auto request(const std::string &url, const lib::headers &headers) -> QNetworkRequest;

		[[deprecated("Use with result callback instead")]]
		void await(QNetworkReply *reply, lib::callback<QByteArray> &callback) const;

		void await(QNetworkReply *reply, lib::callback<lib::result<std::string>> &callback) const;
	};
}
