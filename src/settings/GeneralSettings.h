/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "AbstractSettings.h"

#include <QLocale>
#include <QNetworkProxy>


class test_GeneralSettings;


namespace governikus
{

#if defined(Q_OS_WIN)
	#define GENERAL_SETTINGS_DEFAULT_AUTOSTART true
#else
	#define GENERAL_SETTINGS_DEFAULT_AUTOSTART false
#endif


class GeneralSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class ::test_GeneralSettings;

	private:
		bool mAutoStart;
		QSharedPointer<QSettings> mStore;
		bool mIsNewAppVersion;

		GeneralSettings();
		explicit GeneralSettings(QSharedPointer<QSettings> pStore);
		[[nodiscard]] bool isShowNotificationsOsDefault() const;
		void setAutoStartInternal(bool pAutoStart);
		[[nodiscard]] QString getPersistentSettingsVersion() const;

	public:
		[[nodiscard]] bool isAutoStartAvailable() const;
		[[nodiscard]] bool isAutoStart() const;
		[[nodiscard]] bool autoStartIsSetByAdmin() const;
		[[nodiscard]] bool showTrayIcon() const;
		void setAutoStart(bool pAutoStart);

		[[nodiscard]] bool isNewAppVersion() const;

		[[nodiscard]] bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pAutoClose);

		[[nodiscard]] bool isAutoRedirectAfterAuthentication() const;
		void setAutoRedirectAfterAuthentication(bool pAutoRedirect);

		[[nodiscard]] QString getStartupModule() const;
		void setStartupModule(const QString& pModule);

		[[nodiscard]] bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		[[nodiscard]] bool isTransportPinReminder() const;
		void setTransportPinReminder(bool pTransportPinReminder);

		[[nodiscard]] bool isDeveloperOptions() const;
		void setDeveloperOptions(bool pEnabled);

		[[nodiscard]] bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		[[nodiscard]] bool useSelfAuthTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);

		[[nodiscard]] QLocale::Language getLanguage() const;
		void setLanguage(const QLocale::Language pLanguage);

		[[nodiscard]] bool askForDeviceSurvey() const;
		[[nodiscard]] bool isDeviceSurveyPending() const;
		void setDeviceSurveyPending(bool pDeviceSurveyPending);

		[[nodiscard]] bool askForStoreFeedback() const;
		[[nodiscard]] bool isRequestStoreFeedback() const;
		void setRequestStoreFeedback(bool pRequest);

		[[nodiscard]] QString getLastReaderPluginType() const;
		void setLastReaderPluginType(const QString& pLastReaderPluginType);

		[[nodiscard]] bool isAutoUpdateAvailable() const;
		[[nodiscard]] bool isAutoUpdateCheck() const;
		[[nodiscard]] bool autoUpdateCheckIsSetByAdmin() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);

		[[nodiscard]] bool isVisualPrivacy() const;
		void setVisualPrivacy(bool pVisualPrivacy);

		[[nodiscard]] bool isShuffleScreenKeyboard() const;
		void setShuffleScreenKeyboard(bool pShuffleScreenKeyboard);

		[[nodiscard]] bool isEnableCanAllowed() const;
		void setEnableCanAllowed(bool pEnableCanAllowed);

		[[nodiscard]] bool isSkipRightsOnCanAllowed() const;
		void setSkipRightsOnCanAllowed(bool pSkipRightsOnCanAllowed);

		[[nodiscard]] bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications);

		[[nodiscard]] bool isCustomProxyType() const;
		[[nodiscard]] QNetworkProxy::ProxyType getCustomProxyType() const;

		[[nodiscard]] bool isCustomProxyHost() const;
		[[nodiscard]] QString getCustomProxyHost() const;

		[[nodiscard]] bool isCustomProxyPort() const;
		[[nodiscard]] quint16 getCustomProxyPort() const;

		[[nodiscard]] bool customProxyAttributesPresent() const;
		[[nodiscard]] bool useCustomProxy() const;
		void setUseCustomProxy(bool pUseCustomProxy);

		[[nodiscard]] bool isUseSystemFont() const;
		void setUseSystemFont(bool pUseSystemFont);

		[[nodiscard]] bool isUseAnimations() const;
		void setUseAnimations(bool pUseAnimations);

		[[nodiscard]] QString getDarkMode() const;
		void setDarkMode(const QString& pMode);

		[[nodiscard]] QString getIfdServiceToken();

		[[nodiscard]] bool doSmartUpdate() const;
		[[nodiscard]] bool isSmartAvailable() const;
		void setSmartAvailable(bool pSmartAvailable);

#ifdef Q_OS_WIN
		void migrateSettings();
#endif

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireDeveloperOptionsChanged();
		void fireShowInAppNotificationsChanged();
		void fireProxyChanged();
		void fireUseSystemFontChanged();
		void fireUseAnimationsChanged();
		void fireDarkModeChanged();
		void fireSmartAvailableChanged(bool pSmartAvailable);
};


} // namespace governikus
