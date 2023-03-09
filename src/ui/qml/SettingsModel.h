/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the settings.
 */

#pragma once

#include "AppUpdateDataModel.h"
#include "Env.h"

#include <QObject>
#include <QUrl>
#include <UIPlugIn.h>

namespace governikus
{

class SettingsModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString language READ getLanguage WRITE setLanguage NOTIFY fireLanguageChanged)
	Q_PROPERTY(bool advancedSettings READ isAdvancedSettings WRITE setAdvancedSettings NOTIFY fireAdvancedSettingsChanged)
	Q_PROPERTY(bool developerOptions READ isDeveloperOptions WRITE setDeveloperOptions NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool developerMode READ isDeveloperMode WRITE setDeveloperMode NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool showBetaTesting MEMBER mShowBetaTesting NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool useSelfauthenticationTestUri READ useSelfauthenticationTestUri WRITE setUseSelfauthenticationTestUri NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool pinPadMode READ getPinPadMode WRITE setPinPadMode NOTIFY firePinPadModeChanged)
	Q_PROPERTY(QString serverName READ getServerName WRITE setServerName NOTIFY fireDeviceNameChanged)
	Q_PROPERTY(bool historyEnabled READ isHistoryEnabled WRITE setHistoryEnabled NOTIFY fireHistoryEnabledChanged)
	Q_PROPERTY(bool useScreenKeyboard READ isUseScreenKeyboard WRITE setUseScreenKeyboard NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool visualPrivacy READ isVisualPrivacy WRITE setVisualPrivacy NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool shuffleScreenKeyboard READ isShuffleScreenKeyboard WRITE setShuffleScreenKeyboard NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool enableCanAllowed READ isEnableCanAllowed WRITE setEnableCanAllowed NOTIFY fireCanAllowedChanged)
	Q_PROPERTY(bool skipRightsOnCanAllowed READ isSkipRightsOnCanAllowed WRITE setSkipRightsOnCanAllowed NOTIFY fireCanAllowedChanged)
	Q_PROPERTY(bool enableSimulator READ isSimulatorEnabled WRITE setSimulatorEnabled NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(UiModule startupModule READ getStartupModule WRITE setStartupModule NOTIFY fireStartupModuleChanged)
	Q_PROPERTY(bool autoStartAvailable READ isAutoStartAvailable CONSTANT)
	Q_PROPERTY(bool autoStartApp READ isAutoStart WRITE setAutoStart NOTIFY fireAutoStartChanged)
	Q_PROPERTY(bool autoStartSetByAdmin READ autoStartIsSetByAdmin CONSTANT)
	Q_PROPERTY(bool autoUpdateAvailable READ isAutoUpdateAvailable CONSTANT)
	Q_PROPERTY(bool autoCloseWindowAfterAuthentication READ isAutoCloseWindowAfterAuthentication WRITE setAutoCloseWindowAfterAuthentication NOTIFY fireAutoCloseWindowAfterAuthenticationChanged)
	Q_PROPERTY(bool autoUpdateCheck READ isAutoUpdateCheck WRITE setAutoUpdateCheck NOTIFY fireAutoUpdateCheckChanged)
	Q_PROPERTY(bool autoUpdateCheckSetByAdmin READ autoUpdateCheckIsSetByAdmin CONSTANT)
	Q_PROPERTY(bool remindUserToClose READ isRemindUserToClose WRITE setRemindUserToClose NOTIFY fireRemindUserToCloseChanged)
	Q_PROPERTY(bool transportPinReminder READ isTransportPinReminder WRITE setTransportPinReminder NOTIFY fireTransportPinReminderChanged)
	Q_PROPERTY(bool showInAppNotifications READ isShowInAppNotifications WRITE setShowInAppNotifications NOTIFY fireShowInAppNotificationsChanged)
	Q_PROPERTY(AppUpdateDataModel * appUpdateData READ getAppUpdateData NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl customProxyUrl READ getCustomProxyUrl CONSTANT)
	Q_PROPERTY(bool customProxyAttributesPresent READ isCustomProxyAttributesPresent CONSTANT)
	Q_PROPERTY(bool useCustomProxy READ isUseCustomProxy WRITE setUseCustomProxy NOTIFY fireUseCustomProxyChanged)

	private:
		bool mAdvancedSettings;
		bool mIsStartedByAuth;
		bool mShowBetaTesting;

		SettingsModel();
		~SettingsModel() override = default;

	public:
		[[nodiscard]] QString getLanguage() const;
		void setLanguage(const QString& pLanguage);

		[[nodiscard]] bool isAdvancedSettings() const;
		void setAdvancedSettings(bool pEnabled);

		[[nodiscard]] bool isDeveloperOptions() const;
		void setDeveloperOptions(bool pEnabled);

		[[nodiscard]] bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		[[nodiscard]] bool useSelfauthenticationTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);

		[[nodiscard]] QString getServerName() const;
		void setServerName(const QString& name);

		Q_INVOKABLE void removeTrustedCertificate(const QString& pFingerprint);
		[[nodiscard]] Q_INVOKABLE int removeHistory(const QString& pPeriodToRemove);
		[[nodiscard]] Q_INVOKABLE int removeEntireHistory();

		[[nodiscard]] bool getPinPadMode() const;
		void setPinPadMode(bool pPinPadMode);

		[[nodiscard]] bool isHistoryEnabled() const;
		void setHistoryEnabled(bool pEnabled);

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

		[[nodiscard]] bool isSimulatorEnabled() const;
		void setSimulatorEnabled(bool pEnabled);

		[[nodiscard]] UiModule getStartupModule() const;
		void setStartupModule(UiModule pModule);

		[[nodiscard]] bool isAutoStartAvailable() const;
		[[nodiscard]] bool isAutoStart() const;
		[[nodiscard]] bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pEnabled);

		[[nodiscard]] bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pEnabled);

		[[nodiscard]] bool isAutoUpdateAvailable() const;
		[[nodiscard]] bool isAutoUpdateCheck() const;
		[[nodiscard]] bool autoUpdateCheckIsSetByAdmin() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);

		[[nodiscard]] bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		[[nodiscard]] bool isTransportPinReminder() const;
		void setTransportPinReminder(bool pTransportPinReminder);

		[[nodiscard]] bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications);

		[[nodiscard]] QUrl getCustomProxyUrl() const;
		[[nodiscard]] bool isCustomProxyAttributesPresent() const;
		[[nodiscard]] bool isUseCustomProxy() const;
		void setUseCustomProxy(bool pUseCustomProxy);

		[[nodiscard]] Q_INVOKABLE bool requestStoreFeedback() const;
		Q_INVOKABLE void hideFutureStoreFeedbackDialogs();

		Q_INVOKABLE void updateAppcast();

		[[nodiscard]] AppUpdateDataModel* getAppUpdateData() const;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireAdvancedSettingsChanged();
		void fireDeveloperOptionsChanged();
		void fireDeviceNameChanged();
		void firePinPadModeChanged();
		void fireHistoryEnabledChanged();
		void fireScreenKeyboardChanged();
		void fireCanAllowedChanged();
		void fireStartupModuleChanged();
		void fireAutoStartChanged();
		void fireAutoCloseWindowAfterAuthenticationChanged();
		void fireAutoUpdateCheckChanged();
		void fireRemindUserToCloseChanged();
		void fireTransportPinReminderChanged();
		void fireAppUpdateDataChanged();
		void fireShowInAppNotificationsChanged();
		void fireUseCustomProxyChanged();

};

} // namespace governikus
