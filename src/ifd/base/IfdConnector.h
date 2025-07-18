/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "IfdDescriptor.h"
#include "IfdDispatcherClient.h"

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
		virtual void onConnectRequest(const IfdDescriptor& pIfdDescriptor, const QByteArray& pPsk) = 0;

	Q_SIGNALS:
		void fireDispatcherCreated(const IfdDescriptor& pIfdDescriptor, const QSharedPointer<IfdDispatcherClient>& pClientDispatcher);
		void fireDispatcherError(const IfdDescriptor& pIfdDescriptor, IfdErrorCode pErrorCode);

};

} // namespace governikus
