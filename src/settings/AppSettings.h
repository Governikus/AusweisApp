/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Contains the definition of the AppSettings class.
 */

#pragma once

#include "Env.h"
#include "GeneralSettings.h"
#include "PreVerificationSettings.h"
#include "RemoteServiceSettings.h"

#include <QObject>


namespace governikus
{

/*!
 * \brief Represents application settings.
 */
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

	public:
		virtual GeneralSettings& getGeneralSettings();
		virtual PreVerificationSettings& getPreVerificationSettings();
		virtual RemoteServiceSettings& getRemoteServiceSettings();
};


} // namespace governikus
