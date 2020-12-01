/*!
 * \brief Contains the definition of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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
		bool isShowNotificationsOsDefault() const;
		void setAutoStartInternal(bool pAutoStart);
		QString getPersistentSettingsVersion() const;

	public:
		virtual void save() override;

		bool isAutoStartAvailable() const;
		bool isAutoStart() const;
		bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pAutoStart);

		QString getSkipVersion() const;
		void skipVersion(const QString& pVersion);

		bool isNewAppVersion() const;

		bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pAutoClose);

		bool isShowSetupAssistant() const;
		void setShowSetupAssistant(bool pShowSetupAssistant);

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

		QString getScreenOrientation() const;
		void setScreenOrientation(const QString& pScreenOrientation);

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

		bool isEnableCanAllowed() const;
		void setEnableCanAllowed(bool pEnableCanAllowed);

		bool isSkipRightsOnCanAllowed() const;
		void setSkipRightsOnCanAllowed(bool pSkipRightsOnCanAllowed);

		bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications);

		bool isCustomProxyType() const;
		QNetworkProxy::ProxyType getCustomProxyType() const;

		bool isCustomProxyHost() const;
		QString getCustomProxyHost() const;

		bool isCustomProxyPort() const;
		quint16 getCustomProxyPort() const;

		bool customProxyAttributesPresent() const;
		bool useCustomProxy() const;
		void setUseCustomProxy(bool pUseCustomProxy);

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireDeveloperOptionsChanged();
		void fireShowInAppNotificationsChanged();
		void fireProxyChanged();
};


} // namespace governikus
