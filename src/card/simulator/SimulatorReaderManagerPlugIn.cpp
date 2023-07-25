/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "VolatileSettings.h"


using namespace governikus;


SimulatorReaderManagerPlugIn::SimulatorReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::SIMULATOR, true)
	, mSimulatorReader()
{
	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireDeveloperOptionsChanged, this, &SimulatorReaderManagerPlugIn::onSettingsChanged);
	connect(Env::getSingleton<VolatileSettings>(), &VolatileSettings::fireUsedAsSdkChanged, this, &SimulatorReaderManagerPlugIn::onSettingsChanged);
}


void SimulatorReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();
	onSettingsChanged();
}


QList<Reader*> SimulatorReaderManagerPlugIn::getReaders() const
{
	if (getInfo().isEnabled() && mSimulatorReader)
	{
		return {mSimulatorReader.data()};
	}
	return {};
}


void SimulatorReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	if (getInfo().isEnabled())
	{
		mSimulatorReader.reset(new SimulatorReader());

		connect(mSimulatorReader.data(), &SimulatorReader::fireReaderPropertiesUpdated, this, &SimulatorReaderManagerPlugIn::fireReaderPropertiesUpdated);
		connect(mSimulatorReader.data(), &SimulatorReader::fireCardInserted, this, &SimulatorReaderManagerPlugIn::fireCardInserted);
		connect(mSimulatorReader.data(), &SimulatorReader::fireCardRemoved, this, &SimulatorReaderManagerPlugIn::fireCardRemoved);
		qCDebug(card) << "fireReaderAdded" << mSimulatorReader->getName();
		Q_EMIT fireReaderAdded(mSimulatorReader->getReaderInfo());

		mSimulatorReader->connectReader();
		ReaderManagerPlugIn::startScan(pAutoConnect);
	}
}


void SimulatorReaderManagerPlugIn::stopScan(const QString& pError)
{
	if (mSimulatorReader)
	{
		mSimulatorReader->disconnectReader(pError);

		auto info = mSimulatorReader->getReaderInfo();
		mSimulatorReader.reset();
		Q_EMIT fireReaderRemoved(info);
	}
	ReaderManagerPlugIn::stopScan(pError);
}


void SimulatorReaderManagerPlugIn::insert(const QString& pReaderName, const QVariant& pData)
{
	Q_UNUSED(pReaderName)

	if (!isScanRunning())
	{
		return;
	}

	mSimulatorReader->insertCard(pData);
}


void SimulatorReaderManagerPlugIn::onSettingsChanged()
{
	const bool enabled = Env::getSingleton<AppSettings>()->getGeneralSettings().isSimulatorEnabled() || Env::getSingleton<VolatileSettings>()->isUsedAsSDK();
	if (getInfo().isEnabled() == enabled)
	{
		return;
	}

	qCDebug(card) << "SimulatorStateChanged:" << enabled;
	setPlugInEnabled(enabled);
	if (!enabled && mSimulatorReader)
	{
		mSimulatorReader->disconnect(this);
		const auto& readerInfo = mSimulatorReader->getReaderInfo();
		mSimulatorReader.reset();
		Q_EMIT fireReaderRemoved(readerInfo);
	}
}
