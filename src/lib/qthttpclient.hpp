#pragma once

#include "lib/httpclient.hpp"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCoreApplication>

class QtHttpClient: public QObject, public lib::http_client
{
Q_OBJECT

public:
	explicit QtHttpClient(QObject *parent);

	void get(const std::string &url,
		const lib::headers &headers,
		lib::callback<std::string> &callback) const override;

	void put(const std::string &url, const std::string &body,
		const lib::headers &headers,
		lib::callback<std::string> &callback) const override;

	void post(const std::string &url,
		const lib::headers &headers,
		lib::callback<std::string> &callback) const override;

	auto post(const std::string &url,
		const lib::headers &headers,
		const std::string &post_data) const -> std::string override;

	void del(const std::string &url, const std::string &body,
		const lib::headers &headers,
		lib::callback<std::string> &callback) const override;

private:
	QNetworkAccessManager *networkManager = nullptr;

	static auto request(const std::string &url, const lib::headers &headers) -> QNetworkRequest;

	void await(QNetworkReply *reply, lib::callback<QByteArray> &callback) const;
};
