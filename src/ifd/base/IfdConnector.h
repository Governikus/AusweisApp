/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "IfdDispatcherClient.h"
#include "messages/Discovery.h"

#include <QObject>


namespace governikus
{
defineEnumType(IfdErrorCode,
		INVALID_REQUEST,
		NO_SUPPORTED_API_LEVEL,
		CONNECTION_ERROR,
		REMOTE_HOST_REFUSED_CONNECTION)


class IfdConnector
	: public QObject
{
	Q_OBJECT

	public:
		IfdConnector() = default;
		~IfdConnector() override = default;

		static GlobalStatus errorToGlobalStatus(IfdErrorCode pErrorCode);

	public Q_SLOTS:
		virtual void onConnectRequest(const Discovery& pDiscovery, const QByteArray& pPsk) = 0;

	Q_SIGNALS:
		void fireDispatcherCreated(const QByteArray& pIfdId, const QSharedPointer<IfdDispatcherClient>& pClientDispatcher);
		void fireDispatcherError(const QByteArray& pIfdId, IfdErrorCode pErrorCode);

};

} // namespace governikus
