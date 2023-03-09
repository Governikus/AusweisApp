/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SmartReaderManagerPlugIn.h"

#include "Env.h"
#include "SmartManager.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


bool SmartReaderManagerPlugIn::initializeSmart(const QSharedPointer<SmartManager>& pSmartManager) const
{
	const auto& smartAvailable = isSmartAvailable(pSmartManager);
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK() || smartAvailable)
	{
		return smartAvailable;
	}

	qCDebug(card_smart) << "Updating Smart-eID info";
	pSmartManager->updateInfo();

	return isSmartAvailable(pSmartManager);
}


bool SmartReaderManagerPlugIn::isSmartAvailable(const QSharedPointer<SmartManager>& pSmartManager) const
{
	const auto& eidStatus = pSmartManager->status();
	return eidStatus != EidStatus::INTERNAL_ERROR && eidStatus != EidStatus::UNAVAILABLE;
}


SmartReaderManagerPlugIn::SmartReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::SMART)
	, mSmartReader(nullptr)
{
}


QList<Reader*> SmartReaderManagerPlugIn::getReaders() const
{
	if (mSmartReader)
	{
		return {mSmartReader.data()};
	}

	return {};
}


void SmartReaderManagerPlugIn::init()
{
	if (getInfo().isAvailable())
	{
		return;
	}

	ReaderManagerPlugIn::init();

	const auto& smartManager = SmartManager::get();
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		smartManager->abortSDKWorkflow();
	}

	if (!initializeSmart(smartManager))
	{
		qCWarning(card_smart) << "Smart-eID was not initialized";
		return;
	}

	setPlugInAvailable(true);
	mSmartReader.reset(new SmartReader());
	connect(mSmartReader.data(), &SmartReader::fireReaderPropertiesUpdated, this, &SmartReaderManagerPlugIn::fireReaderPropertiesUpdated);
	connect(mSmartReader.data(), &SmartReader::fireCardInfoChanged, this, &SmartReaderManagerPlugIn::fireCardInfoChanged);
	connect(mSmartReader.data(), &SmartReader::fireCardInserted, this, &SmartReaderManagerPlugIn::fireCardInserted);
	connect(mSmartReader.data(), &SmartReader::fireCardRemoved, this, &SmartReaderManagerPlugIn::fireCardRemoved);
	qCDebug(card_smart) << "Add reader" << mSmartReader->getName();
	Q_EMIT fireReaderAdded(mSmartReader->getReaderInfo());
}


void SmartReaderManagerPlugIn::shutdown()
{
	if (!getInfo().isAvailable())
	{
		return;
	}

	setPlugInAvailable(false);
	mSmartReader.reset();
}


void SmartReaderManagerPlugIn::insert(const QString& pReaderName, const QVariant& pData)
{
	if (!getInfo().isAvailable())
	{
		qCDebug(card_smart) << "Skipping insert because Smart-eID is not available on this device";
		return;
	}

	if (!isScanRunning())
	{
		qCDebug(card_smart) << "Skipping insert because the scan is not running";
		return;
	}

	if (mSmartReader->getReaderInfo().getName() != pReaderName)
	{
		qCDebug(card_smart) << "Skipping insert because of an unexpected reader name";
		return;
	}

	mSmartReader->insertCard(pData);
}


void SmartReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	if (getInfo().isAvailable())
	{
		mSmartReader->connectReader();
		ReaderManagerPlugIn::startScan(pAutoConnect);
	}
}


void SmartReaderManagerPlugIn::stopScan(const QString& pError)
{
	if (getInfo().isAvailable())
	{
		mSmartReader->disconnectReader(pError);
		ReaderManagerPlugIn::stopScan(pError);
	}
}
