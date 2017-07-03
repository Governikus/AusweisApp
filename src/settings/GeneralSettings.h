/*!
 * GeneralSettings.h
 *
 * \brief Contains the definition of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"

#include <QFuture>

class test_GeneralSettings;


namespace governikus
{


#if defined(Q_OS_WIN32) || defined(Q_OS_OSX)
#define GENERAL_SETTINGS_DEFAULT_AUTOSTART true
#else
#define GENERAL_SETTINGS_DEFAULT_AUTOSTART false
#endif

/*!
 * \brief Represents general settings.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
class GeneralSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class::test_GeneralSettings;
	friend bool operator==(const GeneralSettings& pLeft, const GeneralSettings& pRight);

	private:
		/*!
		 * automatically close the window after successful authentications
		 */
		bool mAutoCloseWindowAfterAuthentication;

		/*!
		 * automatically start the program on system start
		 */
		QFuture<bool> mAutoStart;

		/*!
		 * automatically check for updated on program start
		 */
		bool mAutoUpdateCheck;

		/*!
		 * use screen keyboard
		 */
		bool mUseScreenKeyboard;

		/*!
		 * show the setup assistant on application start
		 */
		bool mShowSetupAssistant;

		/*!
		 * remind the user to close the application using the menu and not by clicking the window close button.
		 */
		bool mRemindUserToClose;

		/*!
		 * i.e. version of application instance last saving the settings
		 */
		QString mPersistentSettingsVersion;

		/*!
		 * remind the user to change the transport PIN before first authentication.
		 */
		bool mTransportPinReminder;

		/*!
		 * run the application in developer mode, e.g. failed security checks do not abort operations
		 */
		bool mDeveloperMode;

		/*!
		 * use test uri for selfauthentication to allow test id cards
		 */
		bool mSelfauthenticationTestUri;

		static bool readSettingsAutoStart();
		static void writeSettingsAutoStart(bool pAutoStart);
		GeneralSettings();

	public:
		static void skipVersion(const QString& pVersion);
		static QString getSkipVersion();

		virtual ~GeneralSettings();

		virtual void load() override;
		virtual bool isUnsaved() const override;
		virtual void save() override;

		bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pAutoClose);

		bool isAutoUpdateCheck() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);

		bool isAutoStart() const;
		void setAutoStart(bool pAutoStart);

		bool isUseScreenKeyboard() const;
		void setUseScreenKeyboard(bool pUseScreenKeyboard);

		bool isShowSetupAssistant() const;
		void setShowSetupAssistant(bool pShowSetupAssistant);

		bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		bool isTransportPinReminder() const;
		void setTransportPinReminder(bool pTransportPinReminder);

		const QString& getPersistentSettingsVersion() const;

		bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		bool useSelfauthenticationTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);
};


inline bool operator==(const GeneralSettings& pLeft, const GeneralSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mShowSetupAssistant == pRight.mShowSetupAssistant &&
		pLeft.mRemindUserToClose == pRight.mRemindUserToClose &&
		pLeft.mAutoCloseWindowAfterAuthentication == pRight.mAutoCloseWindowAfterAuthentication &&
		pLeft.mAutoStart.result() == pRight.mAutoStart.result() &&
		pLeft.mAutoUpdateCheck == pRight.mAutoUpdateCheck &&
		pLeft.mUseScreenKeyboard == pRight.mUseScreenKeyboard &&
		pLeft.mPersistentSettingsVersion == pRight.mPersistentSettingsVersion &&
		pLeft.mTransportPinReminder == pRight.mTransportPinReminder &&
		pLeft.mDeveloperMode == pRight.mDeveloperMode &&
		pLeft.mSelfauthenticationTestUri == pRight.mSelfauthenticationTestUri);
}


inline bool operator!=(const GeneralSettings& pLeft, const GeneralSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
