/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDispatcherClient.h"

#include "AppSettings.h"
#include "Initializer.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{
template<> RemoteDispatcherClient* createNewObject<RemoteDispatcherClient*, IfdVersion::Version&&, const QSharedPointer<DataChannel>&>(IfdVersion::Version&& pVersion, const QSharedPointer<DataChannel>& pChannel)
{
	return new RemoteDispatcherClient(pVersion, pChannel);
}


} // namespace governikus


using namespace governikus;


static Initializer::Entry E([] {
			qRegisterMetaType<QSharedPointer<RemoteDispatcherClient> >("QSharedPointer<RemoteDispatcherClient>");
		});


RemoteDispatcherClient::RemoteDispatcherClient(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel)
	: RemoteDispatcher(pVersion, pDataChannel)
{
}


bool RemoteDispatcherClient::processContext(RemoteCardMessageType pMsgType, const QJsonObject& pMsgObject)
{
	if (pMsgType != RemoteCardMessageType::IFDEstablishContextResponse)
	{
		return false;
	}

	IfdEstablishContextResponse establishContextResponse(pMsgObject);
	if (establishContextResponse.resultHasError())
	{
		qCWarning(remote_device) << "Establish context failed with result minor:" << establishContextResponse.getResultMinor();
	}
	else
	{
		mContextHandle = establishContextResponse.getContextHandle();
		qCDebug(remote_device) << "Received new ContextHandle:" << mContextHandle;
		Q_EMIT fireContextEstablished(establishContextResponse.getIfdName(), getId());
	}
	return true;
}


void RemoteDispatcherClient::sendEstablishContext()
{
	qCDebug(remote_device) << "Try to establish context with version" << mVersion;

	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& establishContext = QSharedPointer<const IfdEstablishContext>::create(mVersion, settings.getServerName());
	send(establishContext);
}
