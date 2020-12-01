/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDispatcherServer.h"

#include "AppSettings.h"
#include "Initializer.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"

#include <QLoggingCategory>
#include <QUuid>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{
template<> RemoteDispatcherServer* createNewObject<RemoteDispatcherServer*, const QSharedPointer<DataChannel>&>(const QSharedPointer<DataChannel>& pChannel)
{
	return new RemoteDispatcherServer(pChannel);
}


} // namespace governikus


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<RemoteDispatcherServer> >("QSharedPointer<RemoteDispatcherServer>");
		})


RemoteDispatcherServer::RemoteDispatcherServer(const QSharedPointer<DataChannel>& pDataChannel)
	: RemoteDispatcher(IfdVersion::Version::Unknown, pDataChannel)
{
}


void RemoteDispatcherServer::createAndSendContext(const QJsonObject& pMessageObject)
{
	ECardApiResult::Minor fail = ECardApiResult::Minor::null;

	if (!mContextHandle.isEmpty())
	{
		qCWarning(remote_device) << "Context already established. Additional contexts are not supported";
		fail = ECardApiResult::Minor::AL_Unknown_Error;
	}

	IfdEstablishContext establishContext(pMessageObject);
	if (!establishContext.getProtocol().isSupported())
	{
		qCWarning(remote_device) << "Unsupported API protocol requested:" << establishContext.getProtocolRaw();
		fail = ECardApiResult::Minor::AL_Unknown_Error;
	}

	const auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const QString& serverName = settings.getServerName();
	if (fail != ECardApiResult::Minor::null)
	{
		qCDebug(remote_device) << "Cannot create a new ContextHandle:" << fail;
		send(QSharedPointer<IfdEstablishContextResponse>::create(serverName, fail));
		return;
	}

	mContextHandle = QUuid::createUuid().toString();
	qCDebug(remote_device) << "Creating new ContextHandle:" << mContextHandle;
	send(QSharedPointer<IfdEstablishContextResponse>::create(serverName));
	Q_EMIT fireContextEstablished();
}


bool RemoteDispatcherServer::processContext(RemoteCardMessageType pMsgType, const QJsonObject& pMsgObject)
{
	if (pMsgType != RemoteCardMessageType::IFDEstablishContext)
	{
		return false;
	}

	IfdEstablishContext establishContext(pMsgObject);
	saveRemoteNameInSettings(establishContext.getUdName());
	createAndSendContext(pMsgObject);
	return true;
}
