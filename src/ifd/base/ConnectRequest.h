/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdConnector.h"

#include <QList>
#include <QTimer>
#include <QUrl>
#include <QWebSocket>


namespace governikus
{

class ConnectRequest
	: public QObject
{
	Q_OBJECT

	private:
		const IfdDescriptor mIfdDescriptor;
		QList<QUrl> mAddresses;
		const QByteArray mPsk;
		const QSharedPointer<QWebSocket> mSocket;
		QTimer mTimer;
		bool mRemoteHostRefusedConnection;

		void setTlsConfiguration() const;
		void tryNext();

	private Q_SLOTS:
		void onConnected();
		void onError(QAbstractSocket::SocketError pError);
		void onTimeout();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator) const;
		void onSslErrors(const QList<QSslError>& pErrors);

	public:
		ConnectRequest(const IfdDescriptor& pIfdDescriptor,
				const QByteArray& pPsk,
				int pTimeoutMs);
		~ConnectRequest() override = default;

		[[nodiscard]] const IfdDescriptor& getIfdDescriptor() const;

		void start();

	Q_SIGNALS:
		void fireConnectionCreated(ConnectRequest const* pRequest, const QSharedPointer<QWebSocket>& pWebSocket);
		void fireConnectionError(ConnectRequest const* pRequest, const IfdErrorCode& pError);
};

} // namespace governikus
