/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnectionWorker.h"

#include "MSEBuilder.h"
#include "pace/PaceHandler.h"
#include "ReadBinaryBuilder.h"
#include "ResetRetryCounterBuilder.h"
#include "SelectBuilder.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(support)

CardConnectionWorker::CardConnectionWorker(Reader* pReader)
	: QObject()
	, QEnableSharedFromThis()
	, mReader(pReader)
	, mSecureMessaging()
{
	connect(mReader.data(), &Reader::fireCardInserted, this, &CardConnectionWorker::fireReaderInfoChanged);
	connect(mReader.data(), &Reader::fireCardRemoved, this, &CardConnectionWorker::fireReaderInfoChanged);
	connect(mReader.data(), &Reader::fireCardRetryCounterChanged, this, &CardConnectionWorker::fireReaderInfoChanged);
}


CardConnectionWorker::~CardConnectionWorker()
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (card && card->isConnected())
	{
		card->disconnect();
	}
}


QSharedPointer<CardConnectionWorker> CardConnectionWorker::create(Reader* pReader)
{
	return QSharedPointer<CardConnectionWorker>(new CardConnectionWorker(pReader), &QObject::deleteLater);
}


ReaderInfo CardConnectionWorker::getReaderInfo() const
{
	return mReader.isNull() ? ReaderInfo() : mReader->getReaderInfo();
}


void CardConnectionWorker::setPukInoperative()
{
	mReader->setPukInoperative();
}


QSharedPointer<const EFCardAccess> CardConnectionWorker::getEfCardAccess() const
{
	return getReaderInfo().getCardInfo().getEfCardAccess();
}


ResponseApduResult CardConnectionWorker::transmit(const CommandApdu& pCommandApdu)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return {CardReturnCode::CARD_NOT_FOUND};
	}

	if (mSecureMessaging)
	{
		const CommandApdu securedCommandApdu = mSecureMessaging->encrypt(pCommandApdu);
		if (securedCommandApdu.getBuffer().isEmpty())
		{
			return {CardReturnCode::COMMAND_FAILED};
		}

		ResponseApduResult result = card->transmit(securedCommandApdu);
		result.mResponseApdu = mSecureMessaging->decrypt(result.mResponseApdu);
		if (result.mResponseApdu.isEmpty())
		{
			qCDebug(::card) << "Stopping Secure Messaging since it failed. The channel therefore must no be re-used.";
			stopSecureMessaging();

			return {CardReturnCode::COMMAND_FAILED};
		}
		return result;
	}

	return card->transmit(pCommandApdu);
}


CardReturnCode CardConnectionWorker::readFile(const FileRef& pFileRef, QByteArray& pFileContent)
{
	if (!mReader || !mReader->getCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}

	CommandApdu select = SelectBuilder(pFileRef).build();
	auto [selectReturnCode, selectRes] = transmit(select);
	if (selectReturnCode != CardReturnCode::OK || selectRes.getReturnCode() != StatusCode::SUCCESS)
	{
		return CardReturnCode::COMMAND_FAILED;
	}

	while (true)
	{
		ReadBinaryBuilder rb(static_cast<uint>(pFileContent.count()), 0xff);
		auto [returnCode, res] = transmit(rb.build());
		if (returnCode != CardReturnCode::OK)
		{
			break;
		}

		pFileContent += res.getData();
		const StatusCode statusCode = res.getReturnCode();
		if (statusCode == StatusCode::END_OF_FILE)
		{
			return CardReturnCode::OK;
		}
		if (statusCode != StatusCode::SUCCESS)
		{
			break;
		}
	}

	return CardReturnCode::COMMAND_FAILED;
}


void CardConnectionWorker::setProgressMessage(const QString& pMessage, int pProgress)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (card)
	{
		card->setProgressMessage(pMessage, pProgress);
	}
}


bool CardConnectionWorker::stopSecureMessaging()
{
	if (mSecureMessaging.isNull())
	{
		return false;
	}

	mSecureMessaging.reset();
	return true;
}


EstablishPaceChannelOutput CardConnectionWorker::establishPaceChannel(PacePasswordId pPasswordId,
		const QByteArray& pPasswordValue)
{
	return establishPaceChannel(pPasswordId, pPasswordValue, nullptr, nullptr);
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
		Q_ASSERT(pPasswordValue.isNull());
		output = card->establishPaceChannel(pPasswordId, pChat, pCertificateDescription);
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
		MSEBuilder builder(MSEBuilder::P1::ERASE, MSEBuilder::P2::DEFAULT_CHANNEL);
		return card->transmit(builder.build()).mReturnCode;
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
		ResetRetryCounterBuilder commandBuilder(pNewPin);
		result = transmit(commandBuilder.build());
	}
	else
	{
		Q_ASSERT(pNewPin.isEmpty());
		result = card->setEidPin(pTimeoutSeconds);
	}

	if (result.mReturnCode == CardReturnCode::OK && result.mResponseApdu.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(::card) << "Modify PIN failed";
		return {CardReturnCode::COMMAND_FAILED};
	}
	return result;
}


CardReturnCode CardConnectionWorker::updateRetryCounter()
{
	if (!mReader || !mReader->getCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}
	return mReader->updateRetryCounter(sharedFromThis());
}
