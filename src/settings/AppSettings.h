/*!
 * AppSettings.h
 *
 * \brief Contains the definition of the AppSettings class.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"
#include "DriverSettings.h"
#include "GeneralSettings.h"
#include "HistorySettings.h"
#include "PreVerificationSettings.h"
#include "ProviderSettings.h"
#include "RemoteReaderSettings.h"
#include "SecureStorage.h"

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
	friend bool operator==(const AppSettings& pLeft, const AppSettings& pRight);

	private:
		DriverSettings mDriverSettings;
		GeneralSettings mGeneralSettings;
		PreVerificationSettings mPreVerificationSettings;
		ProviderSettings mProviderSettings;
		SecureStorage mSecureStorage;
		HistorySettings mHistorySettings;
		RemoteReaderSettings mRemoteReaderSettings;

	protected:
		AppSettings();
		virtual ~AppSettings();

	public:
		static AppSettings& getInstance();

		virtual void load() override;
		virtual bool isUnsaved() const override;
		virtual void save() override;

		DriverSettings& getDriverSettings();
		GeneralSettings& getGeneralSettings();
		PreVerificationSettings& getPreVerificationSettings();
		ProviderSettings& getProviderSettings();
		SecureStorage& getSecureStorage();
		HistorySettings& getHistorySettings();
		RemoteReaderSettings& getRemoteReaderSettings();
};

inline bool operator==(const AppSettings& pLeft, const AppSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mDriverSettings == pRight.mDriverSettings &&
		pLeft.mGeneralSettings == pRight.mGeneralSettings &&
		pLeft.mPreVerificationSettings == pRight.mPreVerificationSettings &&
		pLeft.mProviderSettings == pRight.mProviderSettings &&
		pLeft.mSecureStorage == pRight.mSecureStorage &&
		pLeft.mHistorySettings == pRight.mHistorySettings &&
		pLeft.mRemoteReaderSettings == pRight.mRemoteReaderSettings);
}


inline bool operator!=(const AppSettings& pLeft, const AppSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
