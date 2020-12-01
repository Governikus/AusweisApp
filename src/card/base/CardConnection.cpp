/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
#include "CardConnectionWorker.h"

using namespace governikus;

CardConnection::CardConnection(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
	: QObject()
	, mCardConnectionWorker(pCardConnectionWorker)
	, mReaderInfo()
	, mPaceCanSuccessful(false)
	, mPacePinSuccessful(false)
{
	Q_ASSERT(mCardConnectionWorker);
	QMetaObject::invokeMethod(mCardConnectionWorker.data(), &CardConnectionWorker::getReaderInfo, Qt::BlockingQueuedConnection, &mReaderInfo);
	connect(mCardConnectionWorker.data(), &CardConnectionWorker::fireReaderInfoChanged, this, &CardConnection::onReaderInfoChanged);
}


CardConnection::CardConnection()
	: QObject()
	, mCardConnectionWorker()
	, mReaderInfo()
	, mPaceCanSuccessful(false)
	, mPacePinSuccessful(false)
{
}


const ReaderInfo& CardConnection::getReaderInfo()
{
	return mReaderInfo;
}


bool CardConnection::getPaceCanSuccessful() const
{
	return mPaceCanSuccessful;
}


bool CardConnection::getPacePinSuccessful() const
{
	return mPacePinSuccessful;
}


void CardConnection::setProgressMessage(const QString& pMessage, int pProgress)
{
	QMetaObject::invokeMethod(mCardConnectionWorker.data(), [this, pMessage, pProgress] {
				mCardConnectionWorker->setProgressMessage(pMessage, pProgress);
			}, Qt::BlockingQueuedConnection);
}


bool CardConnection::stopSecureMessaging()
{
	bool result;
	QMetaObject::invokeMethod(mCardConnectionWorker.data(), &CardConnectionWorker::stopSecureMessaging, Qt::BlockingQueuedConnection, &result);
	return result;
}


UpdateRetryCounterCommand* CardConnection::createUpdateRetryCounterCommand()
{
	return new UpdateRetryCounterCommand(mCardConnectionWorker);
}


UnblockPinCommand* CardConnection::createUnblockPinCommand(const QByteArray& pPuk)
{
	return new UnblockPinCommand(mCardConnectionWorker, pPuk);
}


EstablishPaceChannelCommand* CardConnection::createEstablishPaceChannelCommand(PacePasswordId pPacePasswordId, const QByteArray& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription)
{
	return new EstablishPaceChannelCommand(mCardConnectionWorker, pPacePasswordId, pPacePassword, pEffectiveChat, pCertificateDescription);
}


SetEidPinCommand* CardConnection::createSetEidPinCommand(const QByteArray& pNewPin, quint8 pTimeoutSeconds)
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
		const CVCertificateChain& pCvcChain, const QByteArray& pEphemeralPublicKeyAsHex,
		const QByteArray& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
{
	return new DidAuthenticateEAC2Command(mCardConnectionWorker, pCvcChain,
			pEphemeralPublicKeyAsHex, pSignatureAsHex, pAuthenticatedAuxiliaryDataAsBinary);
}


void CardConnection::onReaderInfoChanged(const ReaderInfo& pReaderInfo)
{
	mReaderInfo = pReaderInfo;
	Q_EMIT fireReaderInfoChanged(mReaderInfo);
}


TransmitCommand* CardConnection::createTransmitCommand(const QVector<InputAPDUInfo>& pInputApduInfos, const QString& pSlotHandle)
{
	return new TransmitCommand(mCardConnectionWorker, pInputApduInfos, pSlotHandle);
}
