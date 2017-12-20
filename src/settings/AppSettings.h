/*!
 * \brief Contains the definition of the AppSettings class.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"
#include "GeneralSettings.h"
#include "HistorySettings.h"
#include "PreVerificationSettings.h"
#include "RemoteServiceSettings.h"

#include <QObject>

class test_AppSettings;

namespace governikus
{

/*!
 * \brief Represents application settings.
 */
class AppSettings
	: public AbstractSettings
{
	Q_OBJECT
	friend class ::test_AppSettings;

	private:
		GeneralSettings mGeneralSettings;
		PreVerificationSettings mPreVerificationSettings;
		HistorySettings mHistorySettings;
		RemoteServiceSettings mRemoteReaderSettings;

	protected:
		AppSettings();
		virtual ~AppSettings() override;

	public:
		static AppSettings& getInstance();
		virtual void save() override;

		virtual GeneralSettings& getGeneralSettings();
		virtual PreVerificationSettings& getPreVerificationSettings();
		virtual HistorySettings& getHistorySettings();
		virtual RemoteServiceSettings& getRemoteServiceSettings();
};


} /* namespace governikus */
