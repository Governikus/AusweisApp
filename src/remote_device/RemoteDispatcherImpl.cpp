/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDispatcherImpl.h"

#include "AppSettings.h"
#include "Env.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"

#include <QLoggingCategory>
#include <QUuid>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{
template<> RemoteDispatcher* createNewObject<RemoteDispatcher*, const QSharedPointer<DataChannel>&>(const QSharedPointer<DataChannel>& pChannel)
{
	return new RemoteDispatcherImpl(pChannel);
}


} /* namespace governikus */


using namespace governikus;


void RemoteDispatcherImpl::createAndSendContext(const QJsonObject& pMessageObject)
{
	QString fail;

	if (!mContextHandle.isEmpty())
	{
		qCWarning(remote_device) << "Context already established. Additional contexts are not supported";
		fail = QStringLiteral("/al/common#unknownError");
	}

	IfdEstablishContext establishContext(pMessageObject);
	if (!establishContext.getProtocol().isSupported())
	{
		qCWarning(remote_device) << "Unsupported API protocol requested:" << establishContext.getProtocolRaw();
		fail = QStringLiteral("/al/common#unknownError");
	}

	const auto& settings = AppSettings::getInstance().getRemoteServiceSettings();
	const QString& serverName = settings.getServerName();
	if (!fail.isEmpty())
	{
		qCDebug(remote_device) << "Cannot create a new ContextHandle:" << fail;
		IfdEstablishContextResponse response(serverName, fail);
		mDataChannel->send(response.toJson(QUuid::createUuid().toString()).toJson(QJsonDocument::Compact));
		return;
	}

	IfdEstablishContextResponse response(serverName);
	mContextHandle = QUuid::createUuid().toString();
	qCDebug(remote_device) << "Creating new ContextHandle:" << mContextHandle;
	mDataChannel->send(response.toJson(mContextHandle).toJson(QJsonDocument::Compact));
}


void RemoteDispatcherImpl::saveRemoteNameInSettings(const QString& pName)
{
	RemoteServiceSettings& settings = AppSettings::getInstance().getRemoteServiceSettings();
	auto info = settings.getRemoteInfo(getId());
	info.setName(pName);
	settings.updateRemoteInfo(info);
}


void RemoteDispatcherImpl::onReceived(const QByteArray& pDataBlock)
{
	const auto& msgObject = RemoteMessage::parseByteArray(pDataBlock);
	RemoteMessage remoteMessage(msgObject);

	if (remoteMessage.getType() == RemoteCardMessageType::UNDEFINED)
	{
		qCWarning(remote_device) << "Invalid message received:" << pDataBlock;

		const QSharedPointer<const IfdError> errorMessage(new IfdError(QString(), QStringLiteral("/al/common#unknownAPIFunction")));
		send(errorMessage);

		return;
	}

	if (remoteMessage.getType() == RemoteCardMessageType::IFDEstablishContext)
	{
		IfdEstablishContext establishContext(msgObject);

		saveRemoteNameInSettings(establishContext.getUdName());

		createAndSendContext(msgObject);
		return;
	}

	const auto& contextHandle = remoteMessage.getContextHandle();
	if (remoteMessage.getType() == RemoteCardMessageType::IFDEstablishContextResponse)
	{
		IfdEstablishContextResponse establishContextResponse(msgObject);
		if (establishContextResponse.resultHasError())
		{
			qCWarning(remote_device) << "Establish context failed with result minor:" << establishContextResponse.getResultMinor();
			return;
		}

		mContextHandle = establishContextResponse.getContextHandle();
		qCDebug(remote_device) << "Received new ContextHandle:" << mContextHandle;
	}

	if (contextHandle.isEmpty() || mContextHandle != contextHandle)
	{
		qCWarning(remote_device) << "Invalid context handle received. Expecting:" << mContextHandle << "but got:" << pDataBlock;
		return;
	}

	qCDebug(remote_device) << "Received message type:" << remoteMessage.getType();
	Q_EMIT fireReceived(mParser.parse(pDataBlock), sharedFromThis());
}


void RemoteDispatcherImpl::close()
{
	mDataChannel->close();
}


void RemoteDispatcherImpl::onClosed(GlobalStatus::Code pCloseCode)
{
	qCDebug(remote_device) << "Connection closed";

	Q_EMIT fireClosed(pCloseCode, sharedFromThis());
}


RemoteDispatcherImpl::RemoteDispatcherImpl(const QSharedPointer<DataChannel>& pDataChannel)
	: RemoteDispatcher()
	, mDataChannel(pDataChannel)
	, mParser()
{
	connect(mDataChannel.data(), &DataChannel::fireClosed, this, &RemoteDispatcherImpl::onClosed);
	connect(mDataChannel.data(), &DataChannel::fireReceived, this, &RemoteDispatcherImpl::onReceived);
}


RemoteDispatcherImpl::~RemoteDispatcherImpl()
{
	disconnect(mDataChannel.data(), &DataChannel::fireClosed, this, &RemoteDispatcherImpl::onClosed);
	disconnect(mDataChannel.data(), &DataChannel::fireReceived, this, &RemoteDispatcherImpl::onReceived);

	mDataChannel->close();
}


const QString& RemoteDispatcherImpl::getId() const
{
	return mDataChannel->getId();
}


const QString& RemoteDispatcherImpl::getContextHandle() const
{
	return mContextHandle;
}


void RemoteDispatcherImpl::send(const QSharedPointer<const RemoteMessage>& pMessage)
{
	const RemoteCardMessageType messageType = pMessage->getType();

	qCDebug(remote_device) << "Send message of type:" << messageType;
	Q_ASSERT(messageType == RemoteCardMessageType::IFDError || messageType == RemoteCardMessageType::IFDEstablishContext || !mContextHandle.isEmpty());

	mDataChannel->send(pMessage->toJson(mContextHandle).toJson(QJsonDocument::Compact));
}
