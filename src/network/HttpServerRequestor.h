/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Sends one time GET requests to server.
 */

#pragma once

#include "NetworkManager.h"

#include <QEventLoop>
#include <QHostAddress>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QTimer>

class test_UiPluginWebService;

namespace governikus
{

class ServerRequestorManager
	: public NetworkManager
{
	Q_OBJECT

	using NetworkManager::NetworkManager;
};

class HttpServerRequestor
	: public QObject
{
	Q_OBJECT
	friend class ::test_UiPluginWebService;

	private:
		QPointer<NetworkManager> mNetworkManager;
		QEventLoop mEventLoop;
		QTimer mTimer;

		[[nodiscard]] QSharedPointer<QNetworkReply> getRequest(QNetworkRequest pRequest, int pTimeOut = 2000);
		[[nodiscard]] QPointer<NetworkManager> getNetworkManager(bool pCustomNetworkManager);
		[[nodiscard]] QSharedPointer<QNetworkReply> waitForReply(QSharedPointer<QNetworkReply> pReply, int pTimeOut);

	public:
		explicit HttpServerRequestor(bool pCustomNetworkManager = false);
		~HttpServerRequestor() override = default;

		[[nodiscard]] QSharedPointer<QNetworkReply> getRequest(const QUrl& pUrl, int pTimeOut = 2000);
		[[nodiscard]] QSharedPointer<QNetworkReply> postRequest(const QUrl& pUrl, const QByteArray& pData, const QString& pContentType, int pTimeOut = 2000);
		[[nodiscard]] QSharedPointer<QNetworkReply> deleteRequest(const QUrl& pUrl, int pTimeOut = 2000);
		[[nodiscard]] QSharedPointer<QNetworkReply> headRequest(const QUrl& pUrl, int pTimeOut = 2000);
		[[nodiscard]] QSharedPointer<QNetworkReply> optionsRequest(const QUrl& pUrl, int pTimeOut = 2000);

		static QUrl createUrl(const QString& pQuery, quint16 pPort, const QHostAddress& pHost, const QString& pPath = QStringLiteral("/eID-Client"));

	private Q_SLOTS:
		void finished();
};

} // namespace governikus
