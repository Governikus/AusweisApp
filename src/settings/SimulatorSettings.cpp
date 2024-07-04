/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorSettings.h"


using namespace governikus;


namespace
{
SETTINGS_NAME(SETTINGS_GROUP_NAME_SIMULATOR, "simulator")
SETTINGS_NAME(SETTINGS_NAME_ENABLED, "enabled")
SETTINGS_NAME(SETTINGS_NAME_BASIC_READER, "basicReader")
} // namespace


SimulatorSettings::SimulatorSettings()
	: AbstractSettings()
	, mStore(getStore())
{
	// With 2.2.0 the simulator got own settings
	const QAnyStringView simulator("simulator");
	if (mStore->contains(simulator))
	{
		const auto& value = mStore->value(simulator, false).toBool();
		mStore->remove(simulator);
		mStore->beginGroup(SETTINGS_GROUP_NAME_SIMULATOR());
		mStore->setValue(SETTINGS_NAME_ENABLED(), value);
		mStore->endGroup();
	}

	mStore->beginGroup(SETTINGS_GROUP_NAME_SIMULATOR());
}


bool SimulatorSettings::isEnabled() const
{
	return mStore->value(SETTINGS_NAME_ENABLED(), false).toBool();
}


void SimulatorSettings::setEnabled(bool pEnabled)
{
	if (pEnabled != isEnabled())
	{
		mStore->setValue(SETTINGS_NAME_ENABLED(), pEnabled);
		save(mStore);
		Q_EMIT fireEnabledChanged();
	}
}


bool SimulatorSettings::isBasicReader() const
{
	return mStore->value(SETTINGS_NAME_BASIC_READER(), false).toBool();
}


void SimulatorSettings::setBasicReader(bool pBasicReader)
{
	mStore->setValue(SETTINGS_NAME_BASIC_READER(), pBasicReader);
	save(mStore);
}
