/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdDispatcher.h"

#include "AppSettings.h"
#include "messages/IfdError.h"
#include "messages/IfdVersion.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


IfdDispatcher::IfdDispatcher(IfdVersion::Version pVersion, const QSharedPointer<DataChannel>& pDataChannel)
	: QObject()
	, mDataChannel(pDataChannel)
	, mVersion(pVersion)
	, mContextHandle()
{
	Q_ASSERT(mDataChannel);

	connect(mDataChannel.data(), &DataChannel::fireReceived, this, &IfdDispatcher::onReceived);
	connect(mDataChannel.data(), &DataChannel::fireClosed, this, &IfdDispatcher::onClosed);
}


IfdDispatcher::~IfdDispatcher()
{
	disconnect(mDataChannel.data(), &DataChannel::fireReceived, this, &IfdDispatcher::onReceived);
	disconnect(mDataChannel.data(), &DataChannel::fireClosed, this, &IfdDispatcher::onClosed);

	close();
}


void IfdDispatcher::onReceived(const QByteArray& pDataBlock)
{
	const auto& msgObject = IfdMessage::parseByteArray(pDataBlock);
	const IfdMessage ifdMessage(msgObject);
	const IfdMessageType messageType = ifdMessage.getType();

	if (messageType == IfdMessageType::UNDEFINED)
	{
		const QSharedPointer<const IfdError>& errorMessage = QSharedPointer<IfdError>::create(QString(), ECardApiResult::Minor::AL_Unknown_API_Function);
		send(errorMessage);
		return;
	}

	if (processContext(messageType, msgObject))
	{
		return;
	}

	const auto& contextHandle = ifdMessage.getContextHandle();
	if (contextHandle.isEmpty() || mContextHandle != contextHandle)
	{
		qCWarning(ifd) << "Invalid context handle received. Expecting:" << mContextHandle << "but got:" << pDataBlock;
		return;
	}

	qCDebug(ifd) << "Received message type:" << messageType;
	Q_EMIT fireReceived(messageType, msgObject, getId());
}


void IfdDispatcher::onClosed(GlobalStatus::Code pCloseCode)
{
	qCDebug(ifd) << "Connection closed";
	Q_EMIT fireClosed(pCloseCode, getId());
}


void IfdDispatcher::setContextHandle(const QString& pHandle)
{
	mContextHandle = pHandle;
}


void IfdDispatcher::setVersion(IfdVersion::Version pVersion)
{
	mVersion = pVersion;
}


bool IfdDispatcher::isPairingConnection() const
{
	if (!mDataChannel)
	{
		return false;
	}

	return mDataChannel->isPairingConnection();
}


QByteArray IfdDispatcher::getId() const
{
	if (!mDataChannel)
	{
		return QByteArray();
	}

	return mDataChannel->getId();
}


const QString& IfdDispatcher::getContextHandle() const
{
	return mContextHandle;
}


IfdVersion::Version IfdDispatcher::getVersion() const
{
	return mVersion;
}


void IfdDispatcher::saveRemoteNameInSettings(const QString& pName) const
{
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	auto info = settings.getRemoteInfo(getId());
	info.setNameUnescaped(pName);
	settings.updateRemoteInfo(info);
}


void IfdDispatcher::send(const QSharedPointer<const IfdMessage>& pMessage)
{
	Q_ASSERT(mDataChannel);

	const IfdMessageType messageType = pMessage->getType();
	qCDebug(ifd) << "Send message of type:" << messageType << "with context handle:" << mContextHandle;

	Q_ASSERT(!mContextHandle.isEmpty()
			|| messageType == IfdMessageType::IFDError
			|| messageType == IfdMessageType::IFDEstablishContext
			|| messageType == IfdMessageType::IFDEstablishContextResponse);

	mDataChannel->send(pMessage->toByteArray(mVersion, mContextHandle));
}


void IfdDispatcher::close()
{
	if (mDataChannel)
	{
		mDataChannel->close();
	}
}
