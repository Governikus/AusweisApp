/*!
 * \brief Contains the definition of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"

#include <QFuture>
#include <QUuid>

class test_GeneralSettings;


namespace governikus
{

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	#define GENERAL_SETTINGS_DEFAULT_AUTOSTART true
#else
	#define GENERAL_SETTINGS_DEFAULT_AUTOSTART false
#endif


class GeneralSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend bool operator==(const GeneralSettings& pLeft, const GeneralSettings& pRight);

	private:
		QFuture<bool> mAutoStart;
		QSharedPointer<QSettings> mStoreGeneral;
		QSharedPointer<QSettings> mStoreCommon;

		GeneralSettings();
		bool isShowNotificationsOsDefault() const;

	public:
		virtual ~GeneralSettings() override;
		virtual void save() override;

		bool isAutoStartAvailable() const;
		bool isAutoStart() const;
		bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pAutoStart);

		const QString getPersistentSettingsVersion() const;

		QString getSkipVersion();
		void skipVersion(const QString& pVersion);

		bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pAutoClose);

		bool isShowSetupAssistant() const;
		void setShowSetupAssistant(bool pShowSetupAssistant);

		bool isShowNewUiHint() const;
		void setShowNewUiHint(bool pShowNewUiHint);

		bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		bool isTransportPinReminder() const;
		void setTransportPinReminder(bool pTransportPinReminder);

		bool isDeveloperOptions() const;
		void setDeveloperOptions(bool pEnabled);

		bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		bool useSelfAuthTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);

		QLocale::Language getLanguage() const;
		void setLanguage(const QLocale::Language pLanguage);

		QString getSelectedUi() const;
		void setSelectedUi(const QString& pSelectedUi);

		bool askForDeviceSurvey() const;
		bool isDeviceSurveyPending() const;
		void setDeviceSurveyPending(bool pDeviceSurveyPending);

		bool askForStoreFeedback() const;
		bool isRequestStoreFeedback() const;
		void setRequestStoreFeedback(bool pRequest);

		QString getLastReaderPluginType() const;
		void setLastReaderPluginType(const QString& pLastReaderPluginType);

		bool isAutoUpdateAvailable() const;
		bool isAutoUpdateCheck() const;
		bool autoUpdateCheckIsSetByAdmin() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);

		bool isUseScreenKeyboard() const;
		void setUseScreenKeyboard(bool pUseScreenKeyboard);

		bool isVisualPrivacy() const;
		void setVisualPrivacy(bool pVisualPrivacy);

		bool isShuffleScreenKeyboard() const;
		void setShuffleScreenKeyboard(bool pShuffleScreenKeyboard);

		bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications);

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireDeveloperOptionsChanged();
		void fireShowInAppNotificationsChanged();
};


} // namespace governikus
