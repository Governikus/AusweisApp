/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "Env.h"
#include "GeneralSettings.h"
#include "PreVerificationSettings.h"
#include "RemoteServiceSettings.h"
#include "SimulatorSettings.h"

#include <QObject>


namespace governikus
{
class AppSettings
	: public QObject
	, private Env::ThreadSafe
{
	Q_OBJECT
	friend class Env;

	private:
		AppSettings();
		~AppSettings() override = default;

		GeneralSettings mGeneralSettings;
		PreVerificationSettings mPreVerificationSettings;
		RemoteServiceSettings mRemoteReaderSettings;
		SimulatorSettings mSimulatorSettings;

	public:
		[[nodiscard]] GeneralSettings& getGeneralSettings();
		[[nodiscard]] PreVerificationSettings& getPreVerificationSettings();
		[[nodiscard]] RemoteServiceSettings& getRemoteServiceSettings();
		[[nodiscard]] SimulatorSettings& getSimulatorSettings();
};


} // namespace governikus
