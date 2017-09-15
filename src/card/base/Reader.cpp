/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include "asn1/PACEInfo.h"
#include "CardConnectionWorker.h"
#include "PersoSimWorkaround.h"
#include "Reader.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(support)


Reader::Reader(ReaderManagerPlugInType pPlugInType, const QString& pReaderName, ReaderType pType)
	: QObject()
	, mReaderInfo(pPlugInType, pReaderName, pType)
	, mTimerId(0)
	, mUpdateRetryCounter(true)
{
}


Reader::~Reader()
{
}


void Reader::setPukInoperative()
{
	mReaderInfo.mCardInfo.mPukInoperative = true;
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

	if (currentCard->connect() != CardReturnCode::OK)
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
		update();
	}
}


void Reader::update()
{
	CardEvent cardEvent = updateCard();
	fireUpdateSignal(cardEvent);
	updateRetryCounterIfNecessary();
}


void Reader::updateRetryCounterIfNecessary()
{
	if (!mUpdateRetryCounter || mReaderInfo.getCardInfo().getCardType() != CardType::EID_CARD)
	{
		return;
	}

	auto cardConnection = createCardConnectionWorker();
	if (cardConnection)
	{
		updateRetryCounter(cardConnection);
	}
}


CardReturnCode Reader::updateRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
{
	int newRetryCounter = -1;
	bool newPinDeactivated = false;

	CardReturnCode returnCode = getRetryCounter(pCardConnectionWorker, newRetryCounter, newPinDeactivated);
	if (returnCode == CardReturnCode::OK)
	{
		bool changed = (newRetryCounter != mReaderInfo.getRetryCounter()) || (newPinDeactivated != mReaderInfo.isPinDeactivated());
		qCInfo(support) << "retrieved retry counter:" << newRetryCounter << ", was:" << mReaderInfo.getRetryCounter() << ", PIN deactivated:" << newPinDeactivated;
		mUpdateRetryCounter = false;
		mReaderInfo.mCardInfo.mRetryCounter = newRetryCounter;
		mReaderInfo.mCardInfo.mPinDeactivated = newPinDeactivated;

		if (changed)
		{
			qCDebug(card) << "fireCardRetryCounterChanged";
			Q_EMIT fireCardRetryCounterChanged(mReaderInfo.getName());
		}
	}
	return returnCode;
}


CardReturnCode Reader::getRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, int& pRetryCounter, bool& pPinDeactivated)
{
	if (!mReaderInfo.getCardInfo().getEfCardAccess())
	{
		qCCritical(card) << "Cannot get EF.CardAccess";
		return CardReturnCode::COMMAND_FAILED;
	}

	// we don't need to establish PACE with this protocol (i.e. we don't need to support it), so we just take the fist one
	const auto& paceInfo = mReaderInfo.getCardInfo().getEfCardAccess()->getPACEInfos().at(0);
	QByteArray cryptographicMechanismReference = paceInfo->getProtocolValueBytes();
	QByteArray referencePrivateKey = paceInfo->getParameterId();

	PersoSimWorkaround::sendingMseSetAt(pCardConnectionWorker);

	// MSE:Set AT
	MSEBuilder mseBuilder(MSEBuilder::P1::PERFORM_SECURITY_OPERATION, MSEBuilder::P2::SET_AT);
	mseBuilder.setOid(cryptographicMechanismReference);
	mseBuilder.setPublicKey(PACE_PIN_ID::PACE_PIN);
	mseBuilder.setPrivateKey(referencePrivateKey);

	ResponseApdu mseSetAtResponse;
	CardReturnCode returnCode = pCardConnectionWorker->transmit(mseBuilder.build(), mseSetAtResponse);
	if (returnCode != CardReturnCode::OK)
	{
		return returnCode;
	}

	StatusCode statusCode = mseSetAtResponse.getReturnCode();
	qCDebug(card) << "StatusCode: " << statusCode;
	if (statusCode == StatusCode::SUCCESS)
	{
		pRetryCounter = 3;
	}
	else if (statusCode == StatusCode::PIN_RETRY_COUNT_2)
	{
		pRetryCounter = 2;
	}
	else if (statusCode == StatusCode::PIN_SUSPENDED)
	{
		pRetryCounter = 1;
	}
	else if (statusCode == StatusCode::PIN_BLOCKED || statusCode == StatusCode::PIN_DEACTIVATED)
	{
		pRetryCounter = 0;
	}

	pPinDeactivated = statusCode == StatusCode::PIN_DEACTIVATED;

	return CardReturnCode::OK;
}


void Reader::fireUpdateSignal(CardEvent pCardEvent)
{
	switch (pCardEvent)
	{
		case CardEvent::NONE:
			break;

		case CardEvent::CARD_INSERTED:
			qCInfo(support) << "Card inserted of type" << mReaderInfo.getCardType();
			Q_EMIT fireCardInserted(mReaderInfo.getName());
			break;

		case CardEvent::CARD_REMOVED:
			qCInfo(support) << "Card removed";
			Q_EMIT fireCardRemoved(mReaderInfo.getName());
			break;
	}
}


void Reader::onRetryCounterPotentiallyChanged()
{
	mUpdateRetryCounter = true;
}


ConnectableReader::~ConnectableReader()
{
}
