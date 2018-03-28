/*
 * \brief Wrapper around QNetworkAccessManager
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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

		QString getUserAgentHeader() const;

	public Q_SLOTS:
		void onShutdown();

	protected:
		NetworkManager();
		virtual ~NetworkManager();

	public:
		enum class NetworkError
		{
			ServiceUnavailable,
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


		static NetworkManager& getInstance();
		static NetworkError toNetworkError(const QNetworkReply* const pNetworkReply);
		static GlobalStatus toTrustedChannelStatus(const QNetworkReply* const pNetworkReply);
		static GlobalStatus toStatus(const QNetworkReply* const pNetworkReply);
		static QString getTlsVersionString(QSsl::SslProtocol pProtocol);

		virtual void clearConnections();
		virtual QNetworkReply* paos(QNetworkRequest& pRequest,
				const QByteArray& pNamespace,
				const QByteArray& pData,
				bool pUsePsk = true,
				const QByteArray& pSslSession = QByteArray(),
				int pTimeoutInMilliSeconds = 30000);
		virtual QNetworkReply* get(QNetworkRequest& pRequest,
				const QByteArray& pSslSession = QByteArray(),
				int pTimeoutInMilliSeconds = 30000);

		virtual bool checkUpdateServerCertificate(const QNetworkReply& pReply);

		int getOpenConnectionCount();

	Q_SIGNALS:
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		void fireShutdown();
};

} /* namespace governikus */

QDebug operator <<(QDebug pDbg, QSsl::SslProtocol pProtocol);
