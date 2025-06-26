/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdConnector.h"

#include "Initializer.h"

#include <QWebSocket>


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QHostAddress>("QHostAddress");
			qRegisterMetaType<QSharedPointer<QWebSocket>>("QSharedPointer<QWebSocket>");
			qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
		})


GlobalStatus IfdConnector::errorToGlobalStatus(IfdErrorCode pErrorCode)
{
	switch (pErrorCode)
	{
		case IfdErrorCode::INVALID_REQUEST:
			return GlobalStatus::Code::IfdConnector_InvalidRequest;

		case IfdErrorCode::NO_SUPPORTED_API_LEVEL:
			return GlobalStatus::Code::IfdConnector_NoSupportedApiLevel;

		case IfdErrorCode::CONNECTION_ERROR:
			return GlobalStatus::Code::IfdConnector_ConnectionError;

		case IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION:
			return GlobalStatus::Code::IfdConnector_RemoteHostRefusedConnection;
	}

	Q_UNREACHABLE();
}
