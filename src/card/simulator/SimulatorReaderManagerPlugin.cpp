/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorReaderManagerPlugin.h"

#include "AppSettings.h"
#include "VolatileSettings.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_simulator)


SimulatorReaderManagerPlugin::SimulatorReaderManagerPlugin()
	: ReaderManagerPlugin(ReaderManagerPluginType::SIMULATOR, true)
	, mSimulatorReader()
{
	connect(&Env::getSingleton<AppSettings>()->getSimulatorSettings(), &SimulatorSettings::fireEnabledChanged, this, &SimulatorReaderManagerPlugin::onSettingsChanged);
	connect(Env::getSingleton<VolatileSettings>(), &VolatileSettings::fireUsedAsSdkChanged, this, &SimulatorReaderManagerPlugin::onSettingsChanged);
}


void SimulatorReaderManagerPlugin::init()
{
	ReaderManagerPlugin::init();

	onSettingsChanged();
}


QList<Reader*> SimulatorReaderManagerPlugin::getReaders() const
{
	if (getInfo().isEnabled() && mSimulatorReader)
	{
		return {mSimulatorReader.data()};
	}
	return {};
}


void SimulatorReaderManagerPlugin::startScan(bool pAutoConnect)
{
	if (getInfo().isEnabled())
	{
		mSimulatorReader.reset(new SimulatorReader());

		connect(mSimulatorReader.data(), &SimulatorReader::fireReaderPropertiesUpdated, this, &SimulatorReaderManagerPlugin::fireReaderPropertiesUpdated);
		connect(mSimulatorReader.data(), &SimulatorReader::fireCardInserted, this, &SimulatorReaderManagerPlugin::fireCardInserted);
		connect(mSimulatorReader.data(), &SimulatorReader::fireCardRemoved, this, &SimulatorReaderManagerPlugin::fireCardRemoved);
		qCDebug(card_simulator) << "fireReaderAdded" << mSimulatorReader->getName();
		Q_EMIT fireReaderAdded(mSimulatorReader->getReaderInfo());

		mSimulatorReader->connectReader();
		ReaderManagerPlugin::startScan(pAutoConnect);
		setInitialScanState(ReaderManagerPluginInfo::InitialScan::SUCCEEDED);
	}
}


void SimulatorReaderManagerPlugin::stopScan(const QString& pError)
{
	if (mSimulatorReader)
	{
		mSimulatorReader->disconnectReader(pError);

		auto info = mSimulatorReader->getReaderInfo();
		mSimulatorReader.reset();
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

	mSimulatorReader->insertCard(pData);
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
	if (!enabled && mSimulatorReader)
	{
		mSimulatorReader->disconnect(this);
		const auto& readerInfo = mSimulatorReader->getReaderInfo();
		mSimulatorReader.reset();
		Q_EMIT fireReaderRemoved(readerInfo);
	}
}
