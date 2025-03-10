/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ConnectRequest.h"
#include "IfdConnector.h"

#include <QTimer>
#include <QWebSocket>

namespace governikus
{

class IfdConnectorImpl
	: public IfdConnector
{
	Q_OBJECT

	private:
		const int mConnectTimeoutMs;
		QList<QSharedPointer<ConnectRequest>> mPendingRequests;

		void removeRequest(const IfdDescriptor& pIfdDescriptor);

	private Q_SLOTS:
		void onConnectionCreated(const IfdDescriptor& pIfdDescriptor, const QSharedPointer<QWebSocket>& pWebSocket);
		void onConnectionError(const IfdDescriptor& pIfdDescriptor, const IfdErrorCode& pError);
		void onConnectionTimeout(const IfdDescriptor& pIfdDescriptor);

	public:
		explicit IfdConnectorImpl(int pConnectTimeoutMs = 5000);
		~IfdConnectorImpl() override = default;

		void onConnectRequest(const IfdDescriptor& pIfdDescriptor, const QByteArray& pPsk) override;

};

} // namespace governikus
