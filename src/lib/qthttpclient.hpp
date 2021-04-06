#pragma once

#include "lib/httpclient.hpp"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCoreApplication>

class QtHttpClient: public lib::http_client, public QObject
{
public:
	QtHttpClient(lib::settings &settings, QObject *parent);

	void get(const std::string &url, lib::callback<std::string> &callback) override;

	void put(const std::string &url, const std::string &body,
		lib::callback<std::string> &callback) override;

	void post(const std::string &url,
		lib::callback<std::string> &callback) override;

	void del(const std::string &url, const std::string &body,
		lib::callback<std::string> &callback) override;

protected:
	auto post(const std::string &data, const std::string &authorization) -> std::string override;

private:
	QNetworkAccessManager *networkManager = nullptr;

	auto request(const std::string &url) -> QNetworkRequest;

	void await(QNetworkReply *reply, lib::callback<QByteArray> &callback);
};
