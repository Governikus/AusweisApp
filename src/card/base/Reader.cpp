/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include "CardConnectionWorker.h"
#include "PersoSimWorkaround.h"
#include "Reader.h"
#include "asn1/PACEInfo.h"

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

	if (currentCard->connect() != ReturnCode::OK)
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


ReturnCode Reader::updateRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
{
	int newRetryCounter = -1;
	bool newPinDeactivated = false;

	ReturnCode returnCode = getRetryCounter(pCardConnectionWorker, newRetryCounter, newPinDeactivated);
	if (returnCode == ReturnCode::OK)
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


ReturnCode Reader::getRetryCounter(QSharedPointer<CardConnectionWorker> pCardConnectionWorker, int& pRetryCounter, bool& pPinDeactivated)
{
	if (!mReaderInfo.getCardInfo().getEfCardAccess())
	{
		qCCritical(card) << "Cannot get EF.CardAccess";
		return ReturnCode::COMMAND_FAILED;
	}

	// we don't need to establish PACE with this protocol (i.e. we don't need to support it), so we just take the fist one
	QSharedPointer<PACEInfo> paceInfo = mReaderInfo.getCardInfo().getEfCardAccess()->getSecurityInfos<PACEInfo>().at(0);
	QByteArray cryptographicMechanismReference = paceInfo->getProtocolValueBytes();
	QByteArray referencePrivateKey = paceInfo->getParameterId();

	PersoSimWorkaround::sendingMseSetAt(pCardConnectionWorker);

	// MSE:Set AT
	MSEBuilder mseBuilder(MSEBuilder::P1::PERFORM_SECURITY_OPERATION, MSEBuilder::P2::SET_AT);
	mseBuilder.setOid(cryptographicMechanismReference);
	mseBuilder.setPublicKey(PACE_PIN_ID::PACE_PIN);
	mseBuilder.setPrivateKey(referencePrivateKey);

	ResponseApdu mseSetAtResponse;
	ReturnCode returnCode = pCardConnectionWorker->transmit(mseBuilder.build(), mseSetAtResponse);
	if (returnCode != ReturnCode::OK)
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

	return ReturnCode::OK;
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


ConnectableReader::ConnectableReader()
{
}


ConnectableReader::~ConnectableReader()
{
}
