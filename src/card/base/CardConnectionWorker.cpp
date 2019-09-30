/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
	connect(mReader.data(), &Reader::fireCardInserted, this, &CardConnectionWorker::onReaderInfoChanged);
	connect(mReader.data(), &Reader::fireCardRemoved, this, &CardConnectionWorker::onReaderInfoChanged);
	connect(mReader.data(), &Reader::fireCardRetryCounterChanged, this, &CardConnectionWorker::onReaderInfoChanged);
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


void CardConnectionWorker::onReaderInfoChanged(const QString& pReaderName)
{
	Q_ASSERT(pReaderName == mReader->getName());
	Q_EMIT fireReaderInfoChanged(mReader->getReaderInfo());
}


CardReturnCode CardConnectionWorker::transmit(const CommandApdu& pCommandApdu, ResponseApdu& pResponseApdu)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}

	CardReturnCode returnCode;

	if (mSecureMessaging)
	{
		const CommandApdu securedCommandApdu = mSecureMessaging->encrypt(pCommandApdu);
		if (securedCommandApdu.getBuffer().isEmpty())
		{
			return CardReturnCode::COMMAND_FAILED;
		}

		ResponseApdu securedResponseApdu;
		returnCode = card->transmit(securedCommandApdu, securedResponseApdu);
		pResponseApdu = mSecureMessaging->decrypt(securedResponseApdu);
		if (pResponseApdu.isEmpty())
		{
			qCDebug(::card) << "Stopping Secure Messaging since it failed. The channel therefore must no be re-used.";
			stopSecureMessaging();

			return CardReturnCode::COMMAND_FAILED;
		}
	}
	else
	{
		returnCode = card->transmit(pCommandApdu, pResponseApdu);
	}

	return returnCode;
}


CardReturnCode CardConnectionWorker::readFile(const FileRef& pFileRef, QByteArray& pFileContent)
{
	if (!mReader || !mReader->getCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}

	ResponseApdu selectRes;
	CommandApdu select = SelectBuilder(pFileRef).build();
	CardReturnCode returnCode = transmit(select, selectRes);
	if (returnCode != CardReturnCode::OK || selectRes.getReturnCode() != StatusCode::SUCCESS)
	{
		return CardReturnCode::COMMAND_FAILED;
	}

	while (true)
	{
		ResponseApdu res;
		ReadBinaryBuilder rb(static_cast<uint>(pFileContent.count()), 0xff);
		returnCode = transmit(rb.build(), res);
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


void CardConnectionWorker::setProgressMessage(const QString& pMessage)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (card)
	{
		card->setProgressMessage(pMessage);
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
		const QString& pPasswordValue)
{
	return establishPaceChannel(pPasswordId, pPasswordValue, nullptr, nullptr);
}


EstablishPaceChannelOutput CardConnectionWorker::establishPaceChannel(PacePasswordId pPasswordId,
		const QString& pPasswordValue,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return CardReturnCode::CARD_NOT_FOUND;
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
		stopSecureMessaging();
		MSEBuilder builder(MSEBuilder::P1::ERASE, MSEBuilder::P2::DEFAULT_CHANNEL);
		ResponseApdu response;
		CardReturnCode cardReturnCode = card->transmit(builder.build(), response);
		qCDebug(::card) << "Destroying PACE channel with invalid command causing 6700 as return code";
		return cardReturnCode;
	}
	else
	{
		return card->destroyPaceChannel();
	}
}


CardReturnCode CardConnectionWorker::setEidPin(const QString& pNewPin, quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu)
{
	const auto card = mReader ? mReader->getCard() : nullptr;
	if (!card)
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}

	if (mReader->getReaderInfo().isBasicReader())
	{
		Q_ASSERT(!pNewPin.isEmpty());
		ResetRetryCounterBuilder commandBuilder(pNewPin.toUtf8());
		if (transmit(commandBuilder.build(), pResponseApdu) != CardReturnCode::OK || pResponseApdu.getReturnCode() != StatusCode::SUCCESS)
		{
			qCWarning(::card) << "Modify PIN failed";
			return CardReturnCode::COMMAND_FAILED;
		}
		return CardReturnCode::OK;
	}
	else
	{
		Q_ASSERT(pNewPin.isEmpty());
		return card->setEidPin(pTimeoutSeconds, pResponseApdu);
	}
}


CardReturnCode CardConnectionWorker::updateRetryCounter()
{
	if (!mReader || !mReader->getCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}
	return mReader->updateRetryCounter(sharedFromThis());
}
