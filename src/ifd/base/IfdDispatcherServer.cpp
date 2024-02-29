/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDispatcherServer.h"

#include "AppSettings.h"
#include "Initializer.h"
#include "Randomizer.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


namespace governikus
{
template<> IfdDispatcherServer* createNewObject<IfdDispatcherServer*, const QSharedPointer<DataChannel>&>(const QSharedPointer<DataChannel>& pChannel)
{
	return new IfdDispatcherServer(pChannel);
}


} // namespace governikus


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<IfdDispatcherServer>>("QSharedPointer<IfdDispatcherServer>");
		})


IfdDispatcherServer::IfdDispatcherServer(const QSharedPointer<DataChannel>& pDataChannel)
	: IfdDispatcher(IfdVersion::Version::Unknown, pDataChannel)
{
}


void IfdDispatcherServer::createAndSendContext(const QJsonObject& pMessageObject)
{
	ECardApiResult::Minor fail = ECardApiResult::Minor::null;

	if (!getContextHandle().isEmpty())
	{
		qCWarning(ifd) << "Context already established. Additional contexts are not supported";
		fail = ECardApiResult::Minor::AL_Unknown_Error;
	}

	IfdEstablishContext establishContext(pMessageObject);
	const auto& version = establishContext.getProtocol();
	if (!version.isSupported())
	{
		qCWarning(ifd) << "Unsupported API protocol requested:" << establishContext.getProtocolRaw();
		fail = ECardApiResult::Minor::AL_Unknown_Error;
	}
	const auto& ifdVersion = version.getVersion();
	setVersion(ifdVersion);
	qCDebug(ifd) << "Got request to establish context with version" << ifdVersion;

	const auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const QString& serverName = settings.getDeviceName();
	if (fail != ECardApiResult::Minor::null)
	{
		qCDebug(ifd) << "Cannot create a new ContextHandle:" << fail;
		send(QSharedPointer<IfdEstablishContextResponse>::create(serverName, fail));
		return;
	}

	const auto& contextHandle = Randomizer::getInstance().createUuid().toString();
	setContextHandle(contextHandle);
	qCDebug(ifd) << "Creating new ContextHandle:" << contextHandle;
	send(QSharedPointer<IfdEstablishContextResponse>::create(serverName));
	Q_EMIT fireContextEstablished();
}


bool IfdDispatcherServer::processContext(IfdMessageType pMsgType, const QJsonObject& pMsgObject)
{
	if (pMsgType != IfdMessageType::IFDEstablishContext)
	{
		return false;
	}

	IfdEstablishContext establishContext(pMsgObject);
	saveRemoteNameInSettings(establishContext.getUdName());
	Q_EMIT fireNameChanged();
	createAndSendContext(pMsgObject);
	return true;
}
