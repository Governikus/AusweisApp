/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "CardConnectionWorker.h"

using namespace governikus;

CardConnection::CardConnection(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
	: QObject()
	, mCardConnectionWorker(pCardConnectionWorker)
	, mReaderInfo()
{
	Q_ASSERT(mCardConnectionWorker);
	QMetaObject::invokeMethod(mCardConnectionWorker.data(), "getReaderInfo", Qt::BlockingQueuedConnection, Q_RETURN_ARG(ReaderInfo, mReaderInfo));
	connect(mCardConnectionWorker.data(), &CardConnectionWorker::fireReaderInfoChanged, this, &CardConnection::onReaderInfoChanged);
}


CardConnection::~CardConnection()
{
}


const ReaderInfo& CardConnection::getReaderInfo()
{
	return mReaderInfo;
}


bool CardConnection::stopSecureMessaging()
{
	bool result;
	QMetaObject::invokeMethod(mCardConnectionWorker.data(), "stopSecureMessaging", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result));
	return result;
}


UpdateRetryCounterCommand* CardConnection::createUpdateRetryCounterCommand()
{
	return new UpdateRetryCounterCommand(mCardConnectionWorker);
}


UnblockPinCommand* CardConnection::createUnblockPinCommand(const QString& pPuk)
{
	return new UnblockPinCommand(mCardConnectionWorker, pPuk);
}


EstablishPaceChannelCommand* CardConnection::createEstablishPaceChannelCommand(PACE_PASSWORD_ID pPacePasswordId, const QString& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription)
{
	return new EstablishPaceChannelCommand(mCardConnectionWorker, pPacePasswordId, pPacePassword, pEffectiveChat, pCertificateDescription);
}


SetEidPinCommand* CardConnection::createSetEidPinCommand(const QString& pNewPin, quint8 pTimeoutSeconds)
{
	return new SetEidPinCommand(mCardConnectionWorker, pNewPin, pTimeoutSeconds);
}


DestroyPaceChannelCommand* CardConnection::createDestroyPaceChannelCommand()
{
	return new DestroyPaceChannelCommand(mCardConnectionWorker);
}


DidAuthenticateEAC1Command* CardConnection::createDidAuthenticateEAC1Command()
{
	return new DidAuthenticateEAC1Command(mCardConnectionWorker);
}


DidAuthenticateEAC2Command* CardConnection::createDidAuthenticateEAC2Command(
		const CVCertificateChain& pCvcChain, const QString& pEphemeralPublicKeyAsHex,
		const QString& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
{
	return new DidAuthenticateEAC2Command(mCardConnectionWorker, pCvcChain,
			pEphemeralPublicKeyAsHex, pSignatureAsHex, pAuthenticatedAuxiliaryDataAsBinary);
}


void CardConnection::onReaderInfoChanged(const ReaderInfo& pReaderInfo)
{
	mReaderInfo = pReaderInfo;
	Q_EMIT fireReaderInfoChanged(mReaderInfo);
}


TransmitCommand* CardConnection::createTransmitCommand(const QVector<InputAPDUInfo>& pInputApduInfos, const QString pSlotHandle)
{
	return new TransmitCommand(mCardConnectionWorker, pInputApduInfos, pSlotHandle);
}
