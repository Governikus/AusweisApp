/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdConnector.h"

#include <QTimer>
#include <QWebSocket>

namespace governikus
{

class ConnectRequest
	: public QObject
{
	Q_OBJECT

	private:
		const IfdDescriptor mIfdDescriptor;
		const QByteArray mPsk;
		const QSharedPointer<QWebSocket> mSocket;
		QTimer mTimer;

		void setTlsConfiguration() const;

	private Q_SLOTS:
		void onConnected();
		void onError(QAbstractSocket::SocketError pError);
		void onTimeout();
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator);
		void onSslErrors(const QList<QSslError>& pErrors);

	public:
		ConnectRequest(const IfdDescriptor& pIfdDescriptor,
				const QByteArray& pPsk,
				int pTimeoutMs);
		~ConnectRequest() override = default;

		[[nodiscard]] const IfdDescriptor& getIfdDescriptor() const;

		void start();

	Q_SIGNALS:
		void fireConnectionCreated(const IfdDescriptor& pIfdDescriptor,
				const QSharedPointer<QWebSocket>& pWebSocket);
		void fireConnectionError(const IfdDescriptor& pIfdDescriptor, const IfdErrorCode& pError);
		void fireConnectionTimeout(const IfdDescriptor& pIfdDescriptor);
};

} // namespace governikus
