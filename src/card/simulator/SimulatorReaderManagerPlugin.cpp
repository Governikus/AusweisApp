/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorReaderManagerPlugin.h"

#include "AppSettings.h"
#include "VolatileSettings.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_simulator)


SimulatorReaderManagerPlugin::SimulatorReaderManagerPlugin()
	: ReaderManagerPlugin(ReaderManagerPluginType::SIMULATOR, true)
	, mReader()
{
	connect(&Env::getSingleton<AppSettings>()->getSimulatorSettings(), &SimulatorSettings::fireEnabledChanged, this, &SimulatorReaderManagerPlugin::onSettingsChanged);
	connect(Env::getSingleton<VolatileSettings>(), &VolatileSettings::fireUsedAsSdkChanged, this, &SimulatorReaderManagerPlugin::onSettingsChanged);
}


void SimulatorReaderManagerPlugin::init()
{
	ReaderManagerPlugin::init();

	onSettingsChanged();
}


QPointer<Reader> SimulatorReaderManagerPlugin::getReader(const QString& pReaderName) const
{
	if (getInfo().isEnabled() && mReader && mReader->getName() == pReaderName)
	{
		return mReader.data();
	}

	return nullptr;
}


void SimulatorReaderManagerPlugin::startScan(bool pAutoConnect)
{
	if (getInfo().isEnabled())
	{
		mReader.reset(new SimulatorReader());

		connect(mReader.data(), &SimulatorReader::fireReaderPropertiesUpdated, this, &SimulatorReaderManagerPlugin::fireReaderPropertiesUpdated);
		connect(mReader.data(), &SimulatorReader::fireCardInserted, this, &SimulatorReaderManagerPlugin::fireCardInserted);
		connect(mReader.data(), &SimulatorReader::fireCardRemoved, this, &SimulatorReaderManagerPlugin::fireCardRemoved);
		qCDebug(card_simulator) << "fireReaderAdded" << mReader->getName();
		Q_EMIT fireReaderAdded(mReader->getReaderInfo());

		mReader->connectReader();
		ReaderManagerPlugin::startScan(pAutoConnect);
		setInitialScanState(ReaderManagerPluginInfo::InitialScan::SUCCEEDED);
	}
}


void SimulatorReaderManagerPlugin::stopScan(const QString& pError)
{
	if (mReader)
	{
		mReader->disconnectReader(pError);

		auto info = mReader->getReaderInfo();
		mReader.reset();
		Q_EMIT fireReaderRemoved(info);
	}
	ReaderManagerPlugin::stopScan(pError);
}


void SimulatorReaderManagerPlugin::insert(const QString& pReaderName, const QVariant& pData)
{
	Q_UNUSED(pReaderName)

	if (!getInfo().isScanRunning())
	{
		return;
	}

	mReader->insertCard(pData);
}


void SimulatorReaderManagerPlugin::onSettingsChanged()
{
	const bool enabled = Env::getSingleton<AppSettings>()->getSimulatorSettings().isEnabled() || Env::getSingleton<VolatileSettings>()->isUsedAsSDK();
	if (getInfo().isEnabled() == enabled)
	{
		return;
	}

	qCDebug(card_simulator) << "SimulatorStateChanged:" << enabled;
	setPluginEnabled(enabled);
	if (!enabled && mReader)
	{
		mReader->disconnect(this);
		const auto readerInfo = mReader->getReaderInfo();
		mReader.reset();
		Q_EMIT fireReaderRemoved(readerInfo);
	}
}


void SimulatorReaderManagerPlugin::shelveAll() const
{
	if (getInfo().isEnabled() && mReader)
	{
		shelve(mReader.data());
	}
}
