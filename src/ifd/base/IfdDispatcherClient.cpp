/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDispatcherClient.h"

#include "AppSettings.h"
#include "Initializer.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


namespace governikus
{
template<> IfdDispatcherClient* createNewObject<IfdDispatcherClient*, IfdVersion::Version&&, const QSharedPointer<DataChannel>&>(IfdVersion::Version&& pVersion, const QSharedPointer<DataChannel>& pChannel)
{
	return new IfdDispatcherClient(pVersion, pChannel);
}


} // namespace governikus


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<IfdDispatcherClient>>("QSharedPointer<IfdDispatcherClient>");
		})


IfdDispatcherClient::IfdDispatcherClient(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel)
	: IfdDispatcher(pVersion, pDataChannel)
{
}


bool IfdDispatcherClient::processContext(IfdMessageType pMsgType, const QJsonObject& pMsgObject)
{
	if (pMsgType != IfdMessageType::IFDEstablishContextResponse)
	{
		return false;
	}

	IfdEstablishContextResponse establishContextResponse(pMsgObject);
	if (establishContextResponse.resultHasError())
	{
		qCWarning(ifd) << "Establish context failed with result minor:" << establishContextResponse.getResultMinor();
	}
	else
	{

		const auto& contextHandle = establishContextResponse.getContextHandle();
		setContextHandle(contextHandle);
		qCDebug(ifd) << "Received new ContextHandle:" << contextHandle;
		Q_EMIT fireContextEstablished(establishContextResponse.getIfdName(), getId());
	}
	return true;
}


void IfdDispatcherClient::sendEstablishContext()
{
	const auto& ifdVersion = getVersion();
	qCDebug(ifd) << "Try to establish context with version" << ifdVersion;

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const QSharedPointer<const IfdEstablishContext>& establishContext = QSharedPointer<IfdEstablishContext>::create(ifdVersion, settings.getServerName());
	send(establishContext);
}
