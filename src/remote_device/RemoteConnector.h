/*!
 * \brief Module that creates and manages connections from a remote client to a remote server.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "messages/RemoteMessage.h"
#include "RemoteDeviceDescriptor.h"
#include "RemoteDispatcher.h"

#include <QObject>

namespace governikus
{
defineEnumType(RemoteErrorCode,
		INVALID_REQUEST,
		EMPTY_PASSWORD,
		NO_SUPPORTED_API_LEVEL,
		CONNECTION_TIMEOUT,
		CONNECTION_ERROR,
		REMOTE_HOST_REFUSED_CONNECTION)


class RemoteConnector
	: public QObject
{
	Q_OBJECT

	public:
		RemoteConnector();
		virtual ~RemoteConnector() = default;

		static GlobalStatus errorToGlobalStatus(RemoteErrorCode pDeviceError);

	public Q_SLOTS:
		virtual void onConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QString& pPsk) = 0;

	Q_SIGNALS:
		void fireRemoteDispatcherCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QSharedPointer<RemoteDispatcher>& pAdapter);
		void fireRemoteDispatcherError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, RemoteErrorCode pErrorCode);

};

} /* namespace governikus */
