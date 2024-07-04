/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnectionWorker.h"

#include "apdu/CommandApdu.h"
#include "apdu/FileCommand.h"
#include "apdu/PacePinStatus.h"
#include "pace/PaceHandler.h"

#include <QLoggingCategory>
#include <QThread>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(support)


CardConnectionWorker::CardConnectionWorker(Reader* pReader)
	: QObject()
	, QEnableSharedFromThis()
	, mReader(pReader)
	, mSecureMessaging()
	, mKeepAliveTimer()
{
	connect(mReader.data(), &Reader::fireCardInserted, this, &CardConnectionWorker::fireReaderInfoChanged);
	connect(mReader.data(), &Reader::fireCardRemoved, this, &CardConnectionWorker::fireReaderInfoChanged);
	connect(mReader.data(), &Reader::fireCardInfoChanged, this, &CardConnectionWorker::fireReaderInfoChanged);

	mKeepAliveTimer.setInterval(150000);
	connect(&mKeepAliveTimer, &QTimer::timeout, this, &CardConnectionWorker::onKeepAliveTimeout);
}


CardConnectionWorker::~CardConnectionWorker()
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (card && card->isConnected())
	{
		card->releaseConnection();
	}
}


QSharedPointer<CardConnectionWorker> CardConnectionWorker::create(Reader* pReader)
{
	const auto& customDeleter = [](CardConnectionWorker* pWorker){
				if (QThread::currentThread() == pWorker->thread())
				{
					delete pWorker;
				}
				else
				{
					pWorker->deleteLater();
				}
			};

	return QSharedPointer<CardConnectionWorker>(new CardConnectionWorker(pReader), customDeleter);
}


ReaderInfo CardConnectionWorker::getReaderInfo() const
{
	return mReader.isNull() ? ReaderInfo() : mReader->getReaderInfo();
}


void CardConnectionWorker::setPukInoperative()
{
	mReader->setPukInoperative();
}


bool CardConnectionWorker::selectApplicationRoot(const FileRef& pApplication)
{
	qCDebug(card) << "Try to select application:" << pApplication;
	ResponseApduResult select = transmit(FileCommand(pApplication));
	switch (select.mResponseApdu.getStatusCode())
	{
		case StatusCode::SUCCESS:
		case StatusCode::ACCESS_DENIED:
			// According to ICAO Doc 9303 "Machine Readable Travel Documents", Eighth Edition
			// 2021, 4.3.2 "Inspection Process" an unsecured SelectApplication command will
			// return 0x6982 when the card supporting only PACE without Basic Access Control.
			// While productive cards have Basic Access Control the test cards do not have it.
			break;

		case StatusCode::UNSUPPORTED_INS:
			// PersoSim currently does not know its own application identifier.
			break;

		default:
			qCWarning(card) << "Cannot select application:" << select.mResponseApdu.getStatusCode();
			return false;
	}

	qCDebug(card) << "Try to select master file";
	ResponseApduResult result = transmit(FileCommand(FileRef::masterFile()));
	if (result.mResponseApdu.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "Cannot select master file:" << result.mResponseApdu.getStatusCode();
		return false;
	}

	return true;
}


CardReturnCode CardConnectionWorker::updateRetryCounter()
{
	if (!mReader || !mReader->getCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}
	return mReader->updateRetryCounter(sharedFromThis());
}


QSharedPointer<const EFCardAccess> CardConnectionWorker::getEfCardAccess() const
{
	return getReaderInfo().getCardInfo().getEfCardAccess();
}


void CardConnectionWorker::stopSecureMessaging()
{
	mSecureMessaging.reset();
	Q_EMIT fireSecureMessagingStopped();
}


void CardConnectionWorker::onKeepAliveTimeout()
{
	if (!mReader || !mReader->getCard())
	{
		qCDebug(card) << "Keep alive stopped because of targetLost";
		mKeepAliveTimer.stop();
		return;
	}

	if (mReader->getReaderInfo().getCardInfo().getCardType() != CardType::SMART_EID)
	{
		qCDebug(card) << "No Smart-eID - Skipping keep alive";
		return;
	}

	FileCommand command(FileRef::efCardAccess());
	const auto& result = transmit(command);
	if (result.mReturnCode == CardReturnCode::OK)
	{
		qCDebug(card) << "Keep alive successful";
		return;
	}

	qCDebug(card) << "Keep alive failed";
}


ResponseApduResult CardConnectionWorker::transmit(const CommandApdu& pCommandApdu)
{
	if (mSecureMessaging && pCommandApdu.isSecureMessaging())
	{
		qCDebug(::card) << "The eService has established Secure Messaging. Stopping local Secure Messaging.";
		stopSecureMessaging();
	}

	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return {CardReturnCode::CARD_NOT_FOUND};
	}

	CommandApdu commandApdu = pCommandApdu;
	if (mSecureMessaging)
	{
		commandApdu = mSecureMessaging->encrypt(pCommandApdu);
		if (commandApdu.isEmpty())
		{
			return {CardReturnCode::COMMAND_FAILED};
		}
	}

	ResponseApduResult result = card->transmit(commandApdu);
	if (result.mResponseApdu.getStatusCode() == StatusCode::WRONG_LENGTH)
	{
		return {CardReturnCode::WRONG_LENGTH};
	}

	if (mSecureMessaging)
	{
		result.mResponseApdu = mSecureMessaging->decrypt(result.mResponseApdu);
		if (result.mResponseApdu.isEmpty())
		{
			qCDebug(::card) << "Stopping Secure Messaging since it failed. The channel therefore must not be re-used.";
			stopSecureMessaging();

			return {CardReturnCode::COMMAND_FAILED};
		}
	}

	return result;
}


CardReturnCode CardConnectionWorker::readFile(const FileRef& pFileRef, QByteArray& pFileContent, int pLe)
{
	if (!mReader || !mReader->getCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}

	while (true)
	{
		FileCommand command(pFileRef, pFileContent.size(), pLe);
		auto [returnCode, res] = transmit(command);
		if (returnCode != CardReturnCode::OK)
		{
			break;
		}

		const auto& responseData = res.getData();
		pFileContent += responseData;
		switch (res.getStatusCode())
		{
			// Continue, even if the end of the file is probably already reached.
			// There are at least two cases, where we are not able to find it out:
			// 1. The buffer of the card is to small to provide the expected length.
			// 2. The length of the response is less than the expected length
			//    because the maximum length is reduced by secure messaging.
			case StatusCode::SUCCESS:
				if (responseData.isEmpty())
				{
					return CardReturnCode::OK;
				}
				continue;

			// Will probably never happen, because END_OF_FILE is only
			// used for a request with Le < CommandApdu::SHORT_MAX_LE.
			case StatusCode::END_OF_FILE:
			// We hit the case, that the length of the data is a multiple of
			// CommandApdu::SHORT_MAX_LE. The status code of the last response
			// was 0x9000 because the expected bytes could be delivered. So we
			// are not able to decide, if the end of the file was already reached
			// and try to read an additional block.
			// This also happens if a card sends SUCCESS even the whole file was read.
			case StatusCode::ILLEGAL_OFFSET:
				return CardReturnCode::OK;

			default:
				return CardReturnCode::COMMAND_FAILED;
		}
	}

	return CardReturnCode::COMMAND_FAILED;
}


void CardConnectionWorker::setKeepAlive(bool pEnabled)
{
	if (pEnabled)
	{
		mKeepAliveTimer.start();
		return;
	}

	mKeepAliveTimer.stop();
}


void CardConnectionWorker::setProgressMessage(const QString& pMessage, int pProgress)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (card)
	{
		card->setProgressMessage(pMessage, pProgress);
	}
}


EstablishPaceChannelOutput CardConnectionWorker::establishPaceChannel(PacePasswordId pPasswordId,
		const QByteArray& pPasswordValue,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND);
	}

	EstablishPaceChannelOutput output;

	qCInfo(support) << "Starting PACE for" << pPasswordId;
	if (mReader->getReaderInfo().isBasicReader())
	{
		Q_ASSERT(!pPasswordValue.isEmpty());
		PaceHandler paceHandler(sharedFromThis());
		paceHandler.setChat(pChat);
		const auto returnCode = paceHandler.establishPaceChannel(pPasswordId, pPasswordValue);
		output.setPaceReturnCode(returnCode);
		output.setStatusMseSetAt(paceHandler.getStatusMseSetAt());

		if (returnCode == CardReturnCode::OK)
		{
			output.setCarCurr(paceHandler.getCarCurr());
			output.setCarPrev(paceHandler.getCarPrev());
			output.setIdIcc(paceHandler.getIdIcc());
			output.setEfCardAccess(getEfCardAccess()->getContentBytes());
			output.setPaceReturnCode(CardReturnCode::OK);
			mSecureMessaging.reset(new SecureMessaging(paceHandler.getPaceProtocol(), paceHandler.getEncryptionKey(), paceHandler.getMacKey()));
		}
	}
	else
	{
		const bool isTransportPin = (pPasswordValue == QByteArray(5, 0));
		Q_ASSERT(pPasswordValue.isNull() || isTransportPin);
		output = card->establishPaceChannel(pPasswordId, isTransportPin ? 5 : 6, pChat, pCertificateDescription);
	}

	if (output.getPaceReturnCode() == CardReturnCode::INVALID_PASSWORD)
	{
		CardReturnCode invalidPasswordId;
		switch (pPasswordId)
		{
			case PacePasswordId::PACE_CAN:
				invalidPasswordId = CardReturnCode::INVALID_CAN;
				break;

			case PacePasswordId::PACE_PIN:
				switch (PacePinStatus::getRetryCounter(output.getStatusCodeMseSetAt()))
				{
					case 2:
						invalidPasswordId = CardReturnCode::INVALID_PIN_2;
						break;

					case 1:
						invalidPasswordId = CardReturnCode::INVALID_PIN_3;
						break;

					default:
						invalidPasswordId = CardReturnCode::INVALID_PIN;
				}
				break;

			case PacePasswordId::PACE_PUK:
				invalidPasswordId = CardReturnCode::INVALID_PUK;
				break;

			default:
				invalidPasswordId = CardReturnCode::UNKNOWN;
		}
		output.setPaceReturnCode(invalidPasswordId);
	}

	qCInfo(support) << "Finished PACE for" << pPasswordId << "with result" << output.getPaceReturnCode();
	return output;
}


CardReturnCode CardConnectionWorker::destroyPaceChannel()
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}

	qCInfo(support) << "Destroying PACE channel";
	if (mReader->getReaderInfo().isBasicReader())
	{
		qCDebug(::card) << "Destroying PACE channel with invalid command causing 6700 as return code";
		stopSecureMessaging();

		CommandApdu cmdApdu(Ins::MSE_SET, CommandApdu::PACE, CommandApdu::AUTHENTICATION_TEMPLATE);
		return card->transmit(cmdApdu).mReturnCode;
	}
	else
	{
		return card->destroyPaceChannel();
	}
}


ResponseApduResult CardConnectionWorker::setEidPin(const QByteArray& pNewPin, quint8 pTimeoutSeconds)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return {CardReturnCode::CARD_NOT_FOUND};
	}

	ResponseApduResult result;
	if (mReader->getReaderInfo().isBasicReader())
	{
		Q_ASSERT(!pNewPin.isEmpty());
		const CommandApdu cmdApdu(Ins::RESET_RETRY_COUNTER, CommandApdu::CHANGE, CommandApdu::PIN, pNewPin);
		result = transmit(cmdApdu);
	}
	else
	{
		Q_ASSERT(pNewPin.isEmpty());
		result = card->setEidPin(pTimeoutSeconds);
	}

	if (result.mReturnCode == CardReturnCode::OK && result.mResponseApdu.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(::card) << "Modify PIN failed";
	}

	return result;
}


EstablishPaceChannelOutput CardConnectionWorker::prepareIdentification(const QByteArray& pChat) const
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return EstablishPaceChannelOutput(CardReturnCode::CARD_NOT_FOUND);
	}

	const auto& output = card->prepareIdentification(pChat);
	qCInfo(support) << "Finished perform user authentication with result" << output.getPaceReturnCode();

	return output;
}


ResponseApduResult CardConnectionWorker::getChallenge() const
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return {CardReturnCode::CARD_NOT_FOUND};
	}

	ResponseApduResult result = card->getChallenge();

	if (result.mReturnCode != CardReturnCode::OK)
	{
		qCWarning(::card) << "Get challenge failed";
		return {CardReturnCode::COMMAND_FAILED};
	}

	return result;
}


TerminalAndChipAuthenticationResult CardConnectionWorker::performTAandCA(
		const CVCertificateChain& pTerminalCvcChain,
		const QByteArray& pAuxiliaryData,
		const QByteArray& pSignature,
		const QByteArray& pPin,
		const QByteArray& pEphemeralPublicKey) const
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return {CardReturnCode::CARD_NOT_FOUND};
	}

	TerminalAndChipAuthenticationResult result = card->performTAandCA(pTerminalCvcChain, pAuxiliaryData, pSignature, pPin, pEphemeralPublicKey);

	if (result.mReturnCode != CardReturnCode::OK)
	{
		qCWarning(::card) << "Perform terminal and chip authentication failed";
		return {CardReturnCode::COMMAND_FAILED};
	}

	return result;
}
