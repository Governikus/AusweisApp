/*!
 * \brief Contains the definition of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"

#include <QLocale>
#include <QNetworkProxy>
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
	friend class ::test_GeneralSettings;

	private:
		bool mAutoStart;
		QSharedPointer<QSettings> mStoreGeneral;
		QSharedPointer<QSettings> mStoreCommon;
		bool mIsNewAppVersion;

		GeneralSettings();
		GeneralSettings(QSharedPointer<QSettings> pStoreGeneral, QSharedPointer<QSettings> pStoreCommon);
		[[nodiscard]] bool isShowNotificationsOsDefault() const;
		void setAutoStartInternal(bool pAutoStart);
		[[nodiscard]] QString getPersistentSettingsVersion() const;

	public:
		void save() override;

		[[nodiscard]] bool isAutoStartAvailable() const;
		[[nodiscard]] bool isAutoStart() const;
		[[nodiscard]] bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pAutoStart);

		[[nodiscard]] QString getSkipVersion() const;
		void skipVersion(const QString& pVersion);

		[[nodiscard]] bool isNewAppVersion() const;

		[[nodiscard]] bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pAutoClose);

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

		[[nodiscard]] QString getScreenOrientation() const;
		void setScreenOrientation(const QString& pScreenOrientation);

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

		[[nodiscard]] bool isUseScreenKeyboard() const;
		void setUseScreenKeyboard(bool pUseScreenKeyboard);

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

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireDeveloperOptionsChanged();
		void fireShowInAppNotificationsChanged();
		void fireProxyChanged();
};


} // namespace governikus
