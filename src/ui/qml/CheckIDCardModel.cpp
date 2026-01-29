/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CheckIDCardModel.h"

#include "ReaderManager.h"

using namespace governikus;


CheckIDCardModel::CheckIDCardModel(QObject* pParent, int pReaderConnectionTimeout)
	: QObject(pParent)
	, mIsRunning(false)
	, mResult(Result::UNKNOWN)
	, mPluginType(ReaderManagerPluginType::UNKNOWN)
{
	mReaderConnectionWait.setSingleShot(true);
	mReaderConnectionWait.setInterval(pReaderConnectionTimeout);
	connect(&mReaderConnectionWait, &QTimer::timeout, this, &CheckIDCardModel::onReaderConnectionWaitEnded);
}


CheckIDCardModel::~CheckIDCardModel()
{
	stopScan();
}


void CheckIDCardModel::onCardInserted(const ReaderInfo& pInfo)
{
	if (!mReaderWithCard.isEmpty() && pInfo.getName() != mReaderWithCard)
	{
		qWarning() << "Detected conflicting readers with cards";
		return;
	}

	mReaderWithCard = pInfo.getName();

	if (pInfo.hasEid())
	{
		if (pInfo.insufficientApduLength())
		{
			stopScanWithResult(Result::INSUFFICIENT_APDU_LENGTH);
		}
		else if (pInfo.isPinDeactivated())
		{
			stopScanWithResult(Result::PIN_DEACTIVATED);
		}
		else if (pInfo.getRetryCounter() == 1)
		{
			stopScanWithResult(Result::PIN_SUSPENDED);
		}
		else if (pInfo.getRetryCounter() == 0)
		{
			stopScanWithResult(Result::PIN_BLOCKED);
		}
		else
		{
			stopScanWithResult(Result::SUCCESS);
		}
	}
	else if (pInfo.hasCard())
	{
		stopScanWithResult(Result::UNKNOWN_CARD_DETECTED);
	}
}


void CheckIDCardModel::onCardRemoved(const ReaderInfo& pInfo)
{
	if (mReaderWithCard != pInfo.getName())
	{
		return;
	}

	stopScanWithResult(Result::CARD_ACCESS_FAILED);
}


void CheckIDCardModel::onReaderAdded(const ReaderInfo& pInfo)
{
	if (pInfo.hasCard() && pInfo.hasEid())
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
		stopScanWithResult(Result::INSUFFICIENT_APDU_LENGTH);
	}
}


void CheckIDCardModel::onReaderConnectionWaitEnded()
{
	const auto* readerManager = Env::getSingleton<ReaderManager>();
	const auto readers = readerManager->getReaderInfos();
	for (const auto& reader : readers)
	{
		if (reader.hasCard())
		{
			onCardInserted(reader);
		}
	}
}


void CheckIDCardModel::stopScanWithResult(Result result)
{
	qDebug() << "Finish with status" << result;

	mResult = result;
	Q_EMIT fireResultChanged();

	stopScan();
	Q_EMIT fireScanCompleted();
}


void CheckIDCardModel::startScan(ReaderManagerPluginType pPluginType)
{
	if (mIsRunning)
	{
		return;
	}
	static const auto supportedTypes = QList {ReaderManagerPluginType::NFC, ReaderManagerPluginType::PCSC, ReaderManagerPluginType::REMOTE_IFD};
	if (!supportedTypes.contains(pPluginType))
	{
		qWarning() << "Unsupported ReaderManagerPluginType to start scan:" << pPluginType;
		mResult = Result::UNSUPPORTED_PLUGIN_TYPE;
		Q_EMIT fireResultChanged();
		return;
	}

	mIsRunning = true;
	mPluginType = pPluginType;

	const auto* readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireCardInserted, this, &CheckIDCardModel::onCardInserted);
	connect(readerManager, &ReaderManager::fireCardRemoved, this, &CheckIDCardModel::onCardRemoved);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &CheckIDCardModel::onReaderAdded);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &CheckIDCardModel::onReaderRemoved);
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &CheckIDCardModel::onReaderPropertiesUpdated);

	startScanIfNecessary();
}


void CheckIDCardModel::startScanIfNecessary()
{
	if (!mIsRunning)
	{
		return;
	}

	auto* readerManager = Env::getSingleton<ReaderManager>();

	if (readerManager->getPluginInfo(mPluginType).isScanRunning())
	{
		return;
	}

	mReaderWithCard.clear();

	readerManager->startScan(mPluginType);

	const auto readerInfos = readerManager->getReaderInfos(ReaderFilter({mPluginType}));
	mResult = readerInfos.empty() ? Result::NO_READER : Result::CARD_NOT_DETECTED;

	// Directly check all readers if an id card is already present
	for (const auto& info : readerInfos)
	{
		if (info.hasCard() && info.hasEid())
		{
			onReaderAdded(info);
			return;
		}
	}

	mReaderConnectionWait.start();

	Q_EMIT fireResultChanged();
}


void CheckIDCardModel::stopScan()
{
	if (!mIsRunning)
	{
		return;
	}

	mReaderConnectionWait.stop();

	auto* readerManager = Env::getSingleton<ReaderManager>();
	disconnect(readerManager, &ReaderManager::fireCardInserted, this, &CheckIDCardModel::onCardInserted);
	disconnect(readerManager, &ReaderManager::fireCardRemoved, this, &CheckIDCardModel::onCardRemoved);
	disconnect(readerManager, &ReaderManager::fireReaderAdded, this, &CheckIDCardModel::onReaderAdded);
	disconnect(readerManager, &ReaderManager::fireReaderRemoved, this, &CheckIDCardModel::onReaderRemoved);
	disconnect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &CheckIDCardModel::onReaderPropertiesUpdated);

	readerManager->stopScan(mPluginType);

	mIsRunning = false;
	mReaderWithCard.clear();
}


CheckIDCardModel::Result CheckIDCardModel::getResult() const
{
	return mResult;
}
