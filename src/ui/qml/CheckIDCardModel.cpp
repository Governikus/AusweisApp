/*!
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CheckIDCardModel.h"

#include "ReaderManager.h"

using namespace governikus;

CheckIDCardModel::CheckIDCardModel(QObject* pParent)
	: QObject(pParent)
	, mIsRunning(false)
	, mResult(CheckIDCardResult::UNKNOWN)
{
}


CheckIDCardModel::~CheckIDCardModel()
{
	stopScan();
}


void CheckIDCardModel::onCardInserted(const ReaderInfo& pInfo)
{
	if (!mReaderWithCard.isEmpty() && pInfo.getName() != mReaderWithCard)
	{
		qWarning() << "Detected multiple readers with cards";
		return;
	}

	mReaderWithCard = pInfo.getName();

	if (pInfo.hasEid())
	{
		mResult = CheckIDCardResult::ID_CARD_DETECTED;
		Q_EMIT fireResultChanged();

		if (pInfo.insufficientApduLength())
		{
			stopScanWithResult(CheckIDCardResult::INSUFFICIENT_APDU_LENGTH);
		}
		else if (pInfo.isPinDeactivated())
		{
			stopScanWithResult(CheckIDCardResult::PIN_DEACTIVATED);
		}
		else if (pInfo.getRetryCounter() == 1)
		{
			stopScanWithResult(CheckIDCardResult::PIN_SUSPENDED);
		}
		else if (pInfo.getRetryCounter() == 0)
		{
			stopScanWithResult(CheckIDCardResult::PIN_BLOCKED);
		}
		else
		{
			stopScanWithResult(CheckIDCardResult::SUCCESS);
		}
	}
	else if (pInfo.hasCard())
	{
		stopScanWithResult(CheckIDCardResult::UNKNOWN_CARD_DETECTED);
	}
}


void CheckIDCardModel::onCardRemoved(const ReaderInfo& pInfo)
{
	if (mReaderWithCard != pInfo.getName())
	{
		return;
	}

	stopScanWithResult(CheckIDCardResult::CARD_ACCESS_FAILED);
}


void CheckIDCardModel::onReaderAdded(const ReaderInfo& pInfo)
{
	if (pInfo.hasCard())
	{
		onCardInserted(pInfo);
	}
}


void CheckIDCardModel::onReaderRemoved(const ReaderInfo& pInfo)
{
	if (mReaderWithCard != pInfo.getName())
	{
		return;
	}

	onCardRemoved(pInfo);
}


void CheckIDCardModel::onReaderPropertiesUpdated(const ReaderInfo& pInfo)
{
	if (pInfo.insufficientApduLength())
	{
		stopScanWithResult(CheckIDCardResult::INSUFFICIENT_APDU_LENGTH);
	}
}


void CheckIDCardModel::stopScanWithResult(CheckIDCardResult result)
{
	qDebug() << "Finish with status" << result;

	mResult = result;
	Q_EMIT fireResultChanged();

	stopScan();
	Q_EMIT fireScanCompleted();
}


void CheckIDCardModel::startScan()
{
	if (mIsRunning)
	{
		return;
	}

	mIsRunning = true;

	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireCardInserted, this, &CheckIDCardModel::onCardInserted);
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &CheckIDCardModel::onCardRemoved);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &CheckIDCardModel::onReaderAdded);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &CheckIDCardModel::onReaderRemoved);
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &CheckIDCardModel::onReaderPropertiesUpdated);

	startScanIfNecessary();

	Q_EMIT fireIsRunningChanged();
}


void CheckIDCardModel::startScanIfNecessary()
{
	if (!mIsRunning)
	{
		return;
	}

	const auto readerManager = Env::getSingleton<ReaderManager>();

	if (readerManager->isScanRunning(ReaderManagerPlugInType::NFC))
	{
		return;
	}

	mReaderWithCard.clear();

	readerManager->startScan(ReaderManagerPlugInType::NFC);

	const auto nfcReaderInfos = readerManager->getReaderInfos(ReaderFilter({ReaderManagerPlugInType::NFC}));
	mResult = nfcReaderInfos.empty() ? CheckIDCardResult::NO_NFC : CheckIDCardResult::CARD_NOT_DETECTED;

	// Directly check all NFC readers if an id card is already present
	for (const auto& info : nfcReaderInfos)
	{
		if (info.hasCard())
		{
			onReaderAdded(info);
			break;
		}
	}

	Q_EMIT fireResultChanged();
}


void CheckIDCardModel::stopScan()
{
	if (!mIsRunning)
	{
		return;
	}

	const auto readerManager = Env::getSingleton<ReaderManager>();
	disconnect(readerManager, &ReaderManager::fireCardInserted, this, &CheckIDCardModel::onCardInserted);
	disconnect(readerManager, &ReaderManager::fireCardRemoved, this, &CheckIDCardModel::onCardRemoved);
	disconnect(readerManager, &ReaderManager::fireReaderAdded, this, &CheckIDCardModel::onReaderAdded);
	disconnect(readerManager, &ReaderManager::fireReaderRemoved, this, &CheckIDCardModel::onReaderRemoved);
	disconnect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &CheckIDCardModel::onReaderPropertiesUpdated);

	readerManager->stopScan(ReaderManagerPlugInType::NFC);

	mIsRunning = false;
	mReaderWithCard.clear();
	Q_EMIT fireIsRunningChanged();
}


CheckIDCardModel::CheckIDCardResult CheckIDCardModel::getResult() const
{
	return mResult;
}
