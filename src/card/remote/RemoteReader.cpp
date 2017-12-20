/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReader.h"

#include "CardConnectionWorker.h"

#include <QLoggingCategory>
#include <QSignalBlocker>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_remote)


RemoteReader::RemoteReader(const QString& pReaderName, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher, const IfdStatus& pIfdStatus)
	: Reader(ReaderManagerPlugInType::REMOTE, pReaderName)
	, mRemoteDispatcher(pRemoteDispatcher)
{
	mReaderInfo.setBasicReader(!pIfdStatus.getPaceCapabilities().getPace());
	mReaderInfo.setConnected(true);

	update(pIfdStatus);
}


RemoteReader::~RemoteReader()
{
	mCard.reset();
}


Card* RemoteReader::getCard() const
{
	return mCard.data();
}


Reader::CardEvent RemoteReader::updateCard()
{
	return CardEvent::NONE;
}


void RemoteReader::update(const IfdStatus& pIfdStatus)
{
	if (mReaderInfo.getMaxApduLength() != pIfdStatus.getMaxApduLength())
	{
		mReaderInfo.setMaxApduLength(pIfdStatus.getMaxApduLength());
		Q_EMIT fireReaderPropertiesUpdated(getName());

		if (!mReaderInfo.sufficientApduLength())
		{
			qCDebug(card_remote) << "ExtendedLengthApduSupport missing. maxAPDULength:" << mReaderInfo.getMaxApduLength();
			if (!mCard)
			{
				return;
			}
		}
	}

	if (mCard)
	{
		if (!pIfdStatus.getCardAvailable())
		{
			qCDebug(card_remote) << "Card removed";
			mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
			mCard.reset();
			Q_EMIT fireCardRemoved(getName());
		}
		return;
	}

	if (pIfdStatus.getCardAvailable())
	{
		qCDebug(card_remote) << "Card inserted";
		mCard.reset(new RemoteCard(mRemoteDispatcher, getName()));
		QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
		CardInfoFactory::create(cardConnection, mReaderInfo);
		Q_EMIT fireCardInserted(getName());
	}
}
