/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "AbstractSettings.h"


namespace governikus
{

class SimulatorSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;

	private:
		QSharedPointer<QSettings> mStore;

		SimulatorSettings();

	public:
		~SimulatorSettings() override = default;

		[[nodiscard]] bool isEnabled() const;
		void setEnabled(bool pEnabled);

		[[nodiscard]] bool isBasicReader() const;
		void setBasicReader(bool pBasicReader);

	Q_SIGNALS:
		void fireEnabledChanged();
};


} // namespace governikus
