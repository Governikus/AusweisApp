/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Wrapper around QNetworkAccessManager
 */

#pragma once

#include "Env.h"
#include "GlobalStatus.h"
#include "LogHandler.h"

#include <QAtomicInt>
#include <QAuthenticator>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QSsl>

class test_NetworkManager;

namespace governikus
{

class NetworkManager
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_NetworkManager;

	private:
		static bool mLockProxy;

		QNetworkAccessManager mNetAccessManager;
		bool mApplicationExitInProgress;
		QAtomicInt mOpenConnectionCount;
		QSet<QByteArray> mUpdaterSessions;

		bool prepareConnection(QNetworkRequest& pRequest);
		[[nodiscard]] QSharedPointer<QNetworkReply> trackConnection(QNetworkReply* pResponse);
		[[nodiscard]] QSharedPointer<QNetworkReply> processRequest(QNetworkRequest& pRequest,
				const std::function<QSharedPointer<QNetworkReply>(QNetworkRequest&)>& pInvoke);
		[[nodiscard]] QSharedPointer<QNetworkReply> processUpdaterRequest(QNetworkRequest& pRequest,
				const std::function<QSharedPointer<QNetworkReply>(QNetworkRequest&)>& pInvoke);

		[[nodiscard]] QString getUserAgentHeader() const;

	public Q_SLOTS:
		void onShutdown();
		void onProxyChanged();

	protected:
		NetworkManager();
		~NetworkManager() override = default;

	public:
		enum class NetworkError
		{
			ServiceUnavailable,
			TimeOut,
			ProxyError,
			SecurityError,
			OtherError,
		};
		Q_ENUM(NetworkError)

		static void setApplicationProxyFactory();
		static void lockProxy(bool pLocked)
		{
			mLockProxy = pLocked;
		}


		[[nodiscard]] static bool isLoggingAllowed(const QSharedPointer<const QNetworkReply>& pReply);
		[[nodiscard]] static int getLoggedStatusCode(const QSharedPointer<const QNetworkReply>& pReply, const MessageLogger& pLogger);
		[[nodiscard]] static NetworkError toNetworkError(const QSharedPointer<const QNetworkReply>& pNetworkReply);
		[[nodiscard]] static GlobalStatus toTrustedChannelStatus(const QSharedPointer<const QNetworkReply>& pNetworkReply);
		[[nodiscard]] static GlobalStatus toStatus(const QSharedPointer<const QNetworkReply>& pNetworkReply);
		[[nodiscard]] static QString getTlsVersionString(QSsl::SslProtocol pProtocol);
		[[nodiscard]] static QByteArray getStatusMessage(int pStatus);
		[[nodiscard]] static QString getFormattedStatusMessage(int pStatus);

		virtual void clearConnections();
		[[nodiscard]] virtual QSharedPointer<QNetworkReply> paos(QNetworkRequest& pRequest,
				const QByteArray& pNamespace,
				const QByteArray& pData,
				bool pUsePsk = true,
				const QByteArray& pSslSession = QByteArray());
		[[nodiscard]] virtual QSharedPointer<QNetworkReply> get(QNetworkRequest& pRequest);
		[[nodiscard]] virtual QSharedPointer<QNetworkReply> post(QNetworkRequest& pRequest,
				const QByteArray& pData);
		[[nodiscard]] virtual QSharedPointer<QNetworkReply> deleteResource(QNetworkRequest& pRequest);

		[[nodiscard]] QSharedPointer<QNetworkReply> getAsUpdater(QNetworkRequest& pRequest);
		[[nodiscard]] QSharedPointer<QNetworkReply> postAsUpdater(QNetworkRequest& pRequest,
				const QByteArray& pData);

		[[nodiscard]] int getOpenConnectionCount() const;

	Q_SIGNALS:
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		void fireShutdown();
};

} // namespace governikus

QDebug operator <<(QDebug pDbg, QSsl::SslProtocol pProtocol);
