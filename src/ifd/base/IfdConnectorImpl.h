/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ConnectRequest.h"
#include "IfdConnector.h"

#include <QTimer>
#include <QWebSocket>


class test_IfdConnector;


namespace governikus
{

class IfdConnectorImpl
	: public IfdConnector
{
	Q_OBJECT
	friend class ::test_IfdConnector;

	private:
		const int mConnectTimeoutMs;
		QList<QSharedPointer<ConnectRequest>> mPendingRequests;

		QSharedPointer<ConnectRequest> removeRequest(ConnectRequest const* pRequest);

	private Q_SLOTS:
		void onConnectionCreated(ConnectRequest const* pRequest, const QSharedPointer<QWebSocket>& pWebSocket);
		void onConnectionError(ConnectRequest const* pRequest, const IfdErrorCode& pError);

	public:
		explicit IfdConnectorImpl(int pConnectTimeoutMs = 5000);
		~IfdConnectorImpl() override = default;

		void onConnectRequest(const IfdDescriptor& pIfdDescriptor, const QByteArray& pPsk) override;

};

} // namespace governikus
