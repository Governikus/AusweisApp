/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "Reader.h"

#include "CardConnectionWorker.h"
#include "apdu/CommandApdu.h"
#include "apdu/CommandData.h"
#include "apdu/PacePinStatus.h"
#include "asn1/PaceInfo.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(support)


Reader::Reader(ReaderManagerPlugInType pPlugInType, const QString& pReaderName)
	: QObject()
	, mReaderInfo(pReaderName, pPlugInType)
	, mTimerId(0)
{
}


void Reader::setInfoBasicReader(bool pBasicReader)
{
	mReaderInfo.setBasicReader(pBasicReader);
}


void Reader::setInfoMaxApduLength(int pMaxApduLength)
{
	mReaderInfo.setMaxApduLength(pMaxApduLength);
}


void Reader::setInfoCardInfo(const CardInfo& pCardInfo)
{
	mReaderInfo.setCardInfo(pCardInfo);
}


void Reader::setCardInfoTagType(CardInfo::TagType pTagType)
{
	mReaderInfo.getCardInfo().setTagType(pTagType);
}


void Reader::removeCardInfo()
{
	setInfoCardInfo(CardInfo(CardType::NONE));
	mReaderInfo.shelveCard();
}


void Reader::fetchCardInfo(QSharedPointer<CardConnectionWorker> pCardConnection)
{
	setInfoCardInfo(CardInfoFactory::create(pCardConnection));

	if (pCardConnection && pCardConnection->updateRetryCounter() != CardReturnCode::OK)
	{
		qCWarning(card) << "Update of the retry counter failed";
		setInfoCardInfo(CardInfo(CardType::UNKNOWN));
	}
}


int Reader::getTimerId()
{
	return mTimerId;
}


void Reader::setTimerId(int pTimerId)
{
	mTimerId = pTimerId;
}


void Reader::insertCard(const QVariant& pData)
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	const bool skipCheck = pData.type() == QVariant::Bool && pData.toBool();
#else
	const bool skipCheck = pData.typeId() == QMetaType::Bool && pData.toBool();
#endif
	if (!skipCheck && !mReaderInfo.isInsertable())
	{
		qCDebug(card) << "Skipping insert because at least the personalization is missing";
		return;
	}

	if (mReaderInfo.hasCard())
	{
		qCDebug(card) << "Skipping insert because card is already inserted";
		return;
	}

	qCDebug(card) << "Card inserted";
	mReaderInfo.insertCard();
	Q_EMIT fireCardInserted(mReaderInfo);
}


void Reader::shelveCard()
{
	if (mReaderInfo.getCardType() == CardType::NONE)
	{
		return;
	}

	const bool wasShelved = mReaderInfo.wasShelved();

	qCDebug(card) << "Card shelved";
	mReaderInfo.shelveCard();

	if (wasShelved)
	{
		Q_EMIT fireCardRemoved(mReaderInfo);
		return;
	}

	if (mReaderInfo.isInsertable())
	{
		Q_EMIT fireReaderPropertiesUpdated(mReaderInfo);
		return;
	}

	Q_EMIT fireCardInfoChanged(mReaderInfo);
}


void Reader::setPukInoperative()
{
	mReaderInfo.mCardInfo.mPukInoperative = true;
	Q_EMIT fireCardInfoChanged(mReaderInfo);
}


QSharedPointer<CardConnectionWorker> Reader::createCardConnectionWorker()
{
	Card* currentCard = getCard();
	if (currentCard == nullptr)
	{
		qCWarning(card) << "No card available";
		return QSharedPointer<CardConnectionWorker>();
	}

	if (currentCard->isConnected())
	{
		qCWarning(card) << "Card is already connected";
		return QSharedPointer<CardConnectionWorker>();
	}

	if (currentCard->establishConnection() != CardReturnCode::OK)
	{
		qCCritical(card) << "Cannot connect to card";
		return QSharedPointer<CardConnectionWorker>();
	}

	return CardConnectionWorker::create(this);
}


void Reader::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mTimerId)
	{
		updateCard();
	}
}


CardReturnCode Reader::updateRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
{
	auto [returnCode, newRetryCounter, newPinDeactivated, newPinInitial] = getRetryCounter(pCardConnectionWorker);
	if (returnCode == CardReturnCode::OK)
	{
		bool emitSignal = mReaderInfo.isRetryCounterDetermined() && ((newRetryCounter != mReaderInfo.getRetryCounter()) || (newPinDeactivated != mReaderInfo.isPinDeactivated()) || (newPinInitial != mReaderInfo.getCardInfo().isPinInitial()));

		qCInfo(support) << "retrieved retry counter:" << newRetryCounter << ", was:" << mReaderInfo.getRetryCounter() << ", PIN deactivated:" << newPinDeactivated << ", PIN initial: " << newPinInitial;
		mReaderInfo.mCardInfo.mRetryCounter = newRetryCounter;
		mReaderInfo.mCardInfo.mPinDeactivated = newPinDeactivated;
		mReaderInfo.mCardInfo.mPinInitial = newPinInitial;

		if (emitSignal)
		{
			qCDebug(card) << "fireCardInfoChanged";
			Q_EMIT fireCardInfoChanged(mReaderInfo);
		}
	}

	return returnCode;
}


Reader::RetryCounterResult Reader::getRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
{
	if (!mReaderInfo.getCardInfo().getEfCardAccess())
	{
		qCCritical(card) << "Cannot get EF.CardAccess";
		return {CardReturnCode::COMMAND_FAILED};
	}

	// we don't need to establish PACE with this protocol (i.e. we don't need to support it), so we just take the fist one
	const auto& paceInfo = mReaderInfo.getCardInfo().getEfCardAccess()->getPaceInfos().at(0);

	CommandData cmdData;
	cmdData.append(CommandData::CRYPTOGRAPHIC_MECHANISM_REFERENCE, paceInfo->getOid());
	cmdData.append(CommandData::PUBLIC_KEY_REFERENCE, PacePasswordId::PACE_PIN);
	cmdData.append(CommandData::PRIVATE_KEY_REFERENCE, paceInfo->getParameterId());
	CommandApdu cmdApdu(Ins::MSE_SET, CommandApdu::PACE, CommandApdu::AUTHENTICATION_TEMPLATE, cmdData);

	const auto& [returnCode, mseSetAtResponse] = pCardConnectionWorker->transmit(cmdApdu);
	if (returnCode != CardReturnCode::OK)
	{
		return {returnCode};
	}

	const StatusCode statusCode = mseSetAtResponse.getStatusCode();
	qCDebug(card) << "StatusCode:" << statusCode;

	const int retryCounter = PacePinStatus::getRetryCounter(mseSetAtResponse.getStatusCode());
	if (retryCounter == -1)
	{
		return {CardReturnCode::COMMAND_FAILED};
	}

	const bool pinDeactivated = PacePinStatus::isDeactivated(mseSetAtResponse.getStatusCode());
	const bool pinInitial = PacePinStatus::isInitial(mseSetAtResponse.getStatusCode());

	return {CardReturnCode::OK, retryCounter, pinDeactivated, pinInitial};
}
