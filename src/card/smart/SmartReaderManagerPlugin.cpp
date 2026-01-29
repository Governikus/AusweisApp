/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

#include "SmartReaderManagerPlugin.h"

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


void SmartReaderManagerPlugin::publishReader(const ReaderInfo& pInfo)
{
	if (mReaderAdded)
	{
		Q_EMIT fireReaderPropertiesUpdated(pInfo);
		return;
	}

	Q_EMIT fireReaderAdded(pInfo);
	mReaderAdded = true;
}


void SmartReaderManagerPlugin::onSmartAvailableChanged(bool pSmartAvailable)
{
	if (pSmartAvailable)
	{
		init();
		return;
	}

	shutdown();
}


SmartReaderManagerPlugin::SmartReaderManagerPlugin()
	: ReaderManagerPlugin(ReaderManagerPluginType::SMART)
	, mReaderAdded(false)
	, mReader(nullptr)
{
	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireSmartAvailableChanged, this, &SmartReaderManagerPlugin::onSmartAvailableChanged);
}


QPointer<Reader> SmartReaderManagerPlugin::getReader(const QString& pReaderName) const
{
	if (mReader && mReader->getName() == pReaderName)
	{
		return mReader.data();
	}

	return nullptr;
}


void SmartReaderManagerPlugin::init()
{
	ReaderManagerPlugin::init();

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

	setPluginAvailable(true);
	mReader.reset(new SmartReader(READER_NAME()));
	connect(mReader.data(), &SmartReader::fireReaderPropertiesUpdated, this, &SmartReaderManagerPlugin::fireReaderPropertiesUpdated);
	connect(mReader.data(), &SmartReader::fireCardInfoChanged, this, &SmartReaderManagerPlugin::fireCardInfoChanged);
	connect(mReader.data(), &SmartReader::fireCardInserted, this, &SmartReaderManagerPlugin::fireCardInserted);
	connect(mReader.data(), &SmartReader::fireCardRemoved, this, &SmartReaderManagerPlugin::fireCardRemoved);
	qCDebug(card_smart) << "Add reader" << mReader->getName();
	publishReader(mReader->getReaderInfo());
}


void SmartReaderManagerPlugin::shutdown()
{
	if (!getInfo().isAvailable())
	{
		return;
	}

	mReader.reset();
	Q_EMIT fireReaderPropertiesUpdated(ReaderInfo(READER_NAME()));
	setPluginAvailable(false);
}


void SmartReaderManagerPlugin::insert(const QString& pReaderName, const QVariant& pData)
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

	if (mReader->getReaderInfo().getName() != pReaderName)
	{
		qCDebug(card_smart) << "Skipping insert because of an unexpected reader name";
		return;
	}

	mReader->insertCard(pData);
}


void SmartReaderManagerPlugin::startScan(bool pAutoConnect)
{
	if (getInfo().isAvailable())
	{
		mReader->connectReader();
		ReaderManagerPlugin::startScan(pAutoConnect);
	}
}


void SmartReaderManagerPlugin::stopScan(const QString& pError)
{
	if (getInfo().isAvailable())
	{
		mReader->disconnectReader(pError);
		ReaderManagerPlugin::stopScan(pError);
	}
}


void SmartReaderManagerPlugin::shelveAll() const
{
	if (mReader)
	{
		shelve(mReader.data());
	}
}
