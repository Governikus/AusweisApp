/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdConnector.h"

#include <QList>
#include <QTimer>
#include <QUrl>
#include <QWebSocket>


class test_ConnectRequest;


namespace governikus
{

class ConnectRequest
	: public QObject
{
	Q_OBJECT

	friend class ::test_ConnectRequest;

	private:
		const Discovery mDiscovery;
		const QByteArray mPsk;
		QList<QSharedPointer<QWebSocket>> mSockets;
		QTimer mTimer;
		bool mRemoteHostRefusedConnection;

		QSslConfiguration getTlsConfiguration() const;
#ifndef QT_NO_DEBUG
		virtual
#endif
		QAbstractSocket::SocketState getState(const QSharedPointer<QWebSocket>& pSocket) const;
		void processResult(const QSharedPointer<QWebSocket>& pSocket = nullptr);

	private Q_SLOTS:
		void onConnected(const QSharedPointer<QWebSocket>& pSocket);
		void onError(const QSharedPointer<QWebSocket>& pSocket, QAbstractSocket::SocketError pError);
		void onTimeout();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator) const;
		void onSslErrors(const QSharedPointer<QWebSocket>& pSocket, const QList<QSslError>& pErrors) const;

	public:
		ConnectRequest(const Discovery& pDiscovery,
				const QByteArray& pPsk,
				int pTimeoutMs);
		~ConnectRequest() override = default;

		[[nodiscard]] const Discovery& getDiscovery() const;

		void start();

	Q_SIGNALS:
		void fireConnectionCreated(ConnectRequest const* pRequest, const QSharedPointer<QWebSocket>& pWebSocket);
		void fireConnectionError(ConnectRequest const* pRequest, const IfdErrorCode& pError);
};

} // namespace governikus
