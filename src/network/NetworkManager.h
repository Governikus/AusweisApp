/*
 * \brief Wrapper around QNetworkAccessManager
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "GlobalStatus.h"

#include <QAtomicInt>
#include <QAuthenticator>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QSsl>

namespace governikus
{

class NetworkManager
	: public QObject
{
	Q_OBJECT

	private:
		bool mApplicationExitInProgress;
		QAtomicInt mOpenConnectionCount;
		void trackConnection(QNetworkReply* pResponse, const int pTimeoutInMilliSeconds);

		static bool mLockProxy;
		QScopedPointer<QNetworkAccessManager, QScopedPointerDeleteLater> mNetAccessManager;
		QScopedPointer<QNetworkAccessManager, QScopedPointerDeleteLater> mNetAccessManagerPsk;

		void configureProxy(QTcpSocket* pSocket, const QUrl& pUrl);
		QString getUserAgentHeader() const;

	public Q_SLOTS:
		void onShutdown();

	public:
		enum class NetworkError
		{
			TimeOut,
			ProxyError,
			SslError,
			OtherError,
		};
		Q_ENUM(NetworkError)

		static void setApplicationProxyFactory();
		static void lockProxy(bool pLocked)
		{
			mLockProxy = pLocked;
		}


		static NetworkManager& getGlobalInstance();
		static NetworkError toNetworkError(const QNetworkReply* const pNetworkReply);
		static GlobalStatus toTrustedChannelStatus(const QNetworkReply* const pNetworkReply);
		static GlobalStatus toStatus(const QNetworkReply* const pNetworkReply);
		static QString getTlsVersionString(QSsl::SslProtocol pProtocol);

		NetworkManager();
		virtual ~NetworkManager();

		virtual QNetworkReply* paos(QNetworkRequest& pRequest, const QByteArray& pData, bool pUsePsk = true, int pTimeoutInMilliSeconds = 30000);
		virtual QNetworkReply* get(QNetworkRequest& pRequest, int pTimeoutInMilliSeconds = 30000);
		int getOpenConnectionCount();

		/*!
		 * Creates a QSslSocket. The parameter \a pUrl is used to set the proxy for this socket.
		 * The socket is *not* connected.
		 */
		QSslSocket* createSslSocket(const QUrl& pUrl);

	Q_SIGNALS:
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		void fireShutdown();
};

} /* namespace governikus */

QDebug operator <<(QDebug pDbg, QSsl::SslProtocol pProtocol);
