/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "CardConnectionWorker.h"

using namespace governikus;

CardConnection::CardConnection(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
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


UpdateRetryCounterCommand* CardConnection::createUpdateRetryCounterCommand()
{
	return new UpdateRetryCounterCommand(mCardConnectionWorker);
}


UnblockPinCommand* CardConnection::createUnblockPinCommand(const QString& pPuk)
{
	return new UnblockPinCommand(mCardConnectionWorker, pPuk);
}


EstablishPaceChannelCommand* CardConnection::createEstablishPaceChannelCommand(PACE_PIN_ID pPacePinId, const QString& pPacePin, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription)
{
	return new EstablishPaceChannelCommand(mCardConnectionWorker, pPacePinId, pPacePin, pEffectiveChat, pCertificateDescription);
}


SetEidPinCommand* CardConnection::createSetEidPinCommand(const QString& pNewPin, uint pTimeoutSeconds)
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
	Q_EMIT fireReaderInfoChanged();
}


TransmitCommand* CardConnection::createTransmitCommand(const QVector<InputAPDUInfo>& pInputApduInfos)
{
	return new TransmitCommand(mCardConnectionWorker, pInputApduInfos);
}
