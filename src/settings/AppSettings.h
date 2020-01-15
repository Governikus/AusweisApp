/*!
 * \brief Contains the definition of the AppSettings class.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"
#include "Env.h"
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
	, private Env::ThreadSafe
{
	Q_OBJECT
	friend class Env;

	private:
		bool mUsedAsSdk;
		GeneralSettings mGeneralSettings;
		PreVerificationSettings mPreVerificationSettings;
		HistorySettings mHistorySettings;
		RemoteServiceSettings mRemoteReaderSettings;

	protected:
		AppSettings();
		virtual ~AppSettings() override;
		static AppSettings& getInstance();

	public:
		virtual void save() override;
		bool isUsedAsSDK() const;
		void setUsedAsSDK(bool pSdk);

		virtual GeneralSettings& getGeneralSettings();
		virtual PreVerificationSettings& getPreVerificationSettings();
		virtual HistorySettings& getHistorySettings();
		virtual RemoteServiceSettings& getRemoteServiceSettings();
};


} // namespace governikus
