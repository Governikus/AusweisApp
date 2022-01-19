/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteConnector.h"

#include "Initializer.h"

#include <QWebSocket>

using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QHostAddress>("QHostAddress");
			qRegisterMetaType<RemoteErrorCode>("RemoteErrorCode");
			qRegisterMetaType<QSharedPointer<QWebSocket> >("QSharedPointer<QWebSocket>");
			qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
		})


RemoteConnector::RemoteConnector()
{
}


GlobalStatus RemoteConnector::errorToGlobalStatus(RemoteErrorCode pDeviceError)
{
	switch (pDeviceError)
	{
		case RemoteErrorCode::INVALID_REQUEST:
			return GlobalStatus::Code::RemoteConnector_InvalidRequest;

		case RemoteErrorCode::NO_SUPPORTED_API_LEVEL:
			return GlobalStatus::Code::RemoteConnector_NoSupportedApiLevel;

		case RemoteErrorCode::CONNECTION_TIMEOUT:
			return GlobalStatus::Code::RemoteConnector_ConnectionTimeout;

		case RemoteErrorCode::CONNECTION_ERROR:
			return GlobalStatus::Code::RemoteConnector_ConnectionError;

		case RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION:
			return GlobalStatus::Code::RemoteConnector_RemoteHostRefusedConnection;
	}

	Q_UNREACHABLE();
}
