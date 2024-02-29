/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SmartReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "SmartManager.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


static QString READER_NAME()
{
	return QStringLiteral("Smart");
}


void SmartReaderManagerPlugIn::publishReader(const ReaderInfo& pInfo)
{
	if (mReaderAdded)
	{
		Q_EMIT fireReaderPropertiesUpdated(pInfo);
		return;
	}

	Q_EMIT fireReaderAdded(pInfo);
	mReaderAdded = true;
}


void SmartReaderManagerPlugIn::onSmartAvailableChanged(bool pSmartAvailable)
{
	if (pSmartAvailable)
	{
		init();
		return;
	}

	shutdown();
}


SmartReaderManagerPlugIn::SmartReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::SMART)
	, mReaderAdded(false)
	, mSmartReader(nullptr)
{
	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireSmartAvailableChanged, this, &SmartReaderManagerPlugIn::onSmartAvailableChanged);
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
	ReaderManagerPlugIn::init();

	const auto& smartManager = SmartManager::get();
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		smartManager->abortSDKWorkflow();
	}

	if (!smartManager->smartAvailable())
	{
		qCWarning(card_smart) << "Smart-eID is not available";
		publishReader(ReaderInfo(READER_NAME()));
		return;
	}

	if (getInfo().isAvailable())
	{
		return;
	}

	setPlugInAvailable(true);
	mSmartReader.reset(new SmartReader(READER_NAME()));
	connect(mSmartReader.data(), &SmartReader::fireReaderPropertiesUpdated, this, &SmartReaderManagerPlugIn::fireReaderPropertiesUpdated);
	connect(mSmartReader.data(), &SmartReader::fireCardInfoChanged, this, &SmartReaderManagerPlugIn::fireCardInfoChanged);
	connect(mSmartReader.data(), &SmartReader::fireCardInserted, this, &SmartReaderManagerPlugIn::fireCardInserted);
	connect(mSmartReader.data(), &SmartReader::fireCardRemoved, this, &SmartReaderManagerPlugIn::fireCardRemoved);
	qCDebug(card_smart) << "Add reader" << mSmartReader->getName();
	publishReader(mSmartReader->getReaderInfo());
}


void SmartReaderManagerPlugIn::shutdown()
{
	if (!getInfo().isAvailable())
	{
		return;
	}

	mSmartReader.reset();
	Q_EMIT fireReaderPropertiesUpdated(ReaderInfo(READER_NAME()));
	setPlugInAvailable(false);
}


void SmartReaderManagerPlugIn::insert(const QString& pReaderName, const QVariant& pData)
{
	if (!getInfo().isAvailable())
	{
		qCDebug(card_smart) << "Skipping insert because Smart-eID is not available on this device";
		return;
	}

	if (!getInfo().isScanRunning())
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
