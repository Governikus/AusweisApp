/*!
 * AppSettings.h
 *
 * \brief Contains the definition of the AppSettings class.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>

#include "AbstractSettings.h"
#include "DriverSettings.h"
#include "GeneralSettings.h"
#include "HistorySettings.h"
#include "PreVerificationSettings.h"
#include "ProviderSettings.h"
#include "ProxySettings.h"
#include "SecureStorage.h"

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
		ProxySettings mProxySettings;
		SecureStorage mSecureStorage;
		HistorySettings mHistorySettings;

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
		ProxySettings& getProxySettings();
		SecureStorage& getSecureStorage();
		HistorySettings& getHistorySettings();
};

inline bool operator==(const AppSettings& pLeft, const AppSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mDriverSettings == pRight.mDriverSettings &&
		pLeft.mGeneralSettings == pRight.mGeneralSettings &&
		pLeft.mPreVerificationSettings == pRight.mPreVerificationSettings &&
		pLeft.mProviderSettings == pRight.mProviderSettings &&
		pLeft.mProxySettings == pRight.mProxySettings &&
		pLeft.mSecureStorage == pRight.mSecureStorage &&
		pLeft.mHistorySettings == pRight.mHistorySettings);
}


inline bool operator!=(const AppSettings& pLeft, const AppSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
