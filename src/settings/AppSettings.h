/*!
 * \brief Contains the definition of the AppSettings class.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "GeneralSettings.h"
#include "HistorySettings.h"
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
		HistorySettings mHistorySettings;
		RemoteServiceSettings mRemoteReaderSettings;

	public:
		virtual GeneralSettings& getGeneralSettings();
		virtual PreVerificationSettings& getPreVerificationSettings();
		virtual HistorySettings& getHistorySettings();
		virtual RemoteServiceSettings& getRemoteServiceSettings();
};


} // namespace governikus
