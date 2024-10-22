/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the settings.
 */

#pragma once

#include "AppUpdateDataModel.h"
#include "Env.h"
#include "SingletonCreator.h"
#include "UiPlugin.h"

#include <QObject>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

namespace governikus
{

class SettingsModel
	: public QObject
	, public SingletonCreator<SettingsModel>
{
	Q_OBJECT
	Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;

	Q_PROPERTY(QString language READ getLanguage WRITE setLanguage NOTIFY fireLanguageChanged)
	Q_PROPERTY(bool advancedSettings READ isAdvancedSettings WRITE setAdvancedSettings NOTIFY fireAdvancedSettingsChanged)
	Q_PROPERTY(bool developerOptions READ isDeveloperOptions WRITE setDeveloperOptions NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool developerMode READ isDeveloperMode WRITE setDeveloperMode NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool showBetaTesting MEMBER mShowBetaTesting NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool useSelfauthenticationTestUri READ useSelfauthenticationTestUri WRITE setUseSelfauthenticationTestUri NOTIFY fireDeveloperOptionsChanged)
	Q_PROPERTY(bool pinPadMode READ getPinPadMode WRITE setPinPadMode NOTIFY firePinPadModeChanged)
	Q_PROPERTY(bool showAccessRights READ getShowAccessRights WRITE setShowAccessRights NOTIFY fireShowAccessRightsChanged)
	Q_PROPERTY(QString deviceName READ getDeviceName WRITE setDeviceName NOTIFY fireDeviceNameChanged)
	Q_PROPERTY(bool visualPrivacy READ isVisualPrivacy WRITE setVisualPrivacy NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool shuffleScreenKeyboard READ isShuffleScreenKeyboard WRITE setShuffleScreenKeyboard NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool enableCanAllowed READ isEnableCanAllowed WRITE setEnableCanAllowed NOTIFY fireCanAllowedChanged)
	Q_PROPERTY(bool skipRightsOnCanAllowed READ isSkipRightsOnCanAllowed WRITE setSkipRightsOnCanAllowed NOTIFY fireCanAllowedChanged)
	Q_PROPERTY(bool enableSimulator READ isSimulatorEnabled WRITE setSimulatorEnabled NOTIFY fireSimulatorChanged)
	Q_PROPERTY(governikus::EnumUiModule::UiModule startupModule READ getStartupModule WRITE setStartupModule NOTIFY fireStartupModuleChanged)
	Q_PROPERTY(bool autoStartAvailable READ isAutoStartAvailable CONSTANT)
	Q_PROPERTY(bool autoStartApp READ isAutoStart WRITE setAutoStart NOTIFY fireAutoStartChanged)
	Q_PROPERTY(bool showTrayIcon READ showTrayIcon NOTIFY fireShowTrayIconChanged)
	Q_PROPERTY(bool autoStartSetByAdmin READ autoStartIsSetByAdmin CONSTANT)
	Q_PROPERTY(bool autoUpdateAvailable READ isAutoUpdateAvailable CONSTANT)
	Q_PROPERTY(bool autoCloseWindowAfterAuthentication READ isAutoCloseWindowAfterAuthentication WRITE setAutoCloseWindowAfterAuthentication NOTIFY fireAutoCloseWindowAfterAuthenticationChanged)
	Q_PROPERTY(bool autoRedirectAfterAuthentication READ isAutoRedirectAfterAuthentication WRITE setAutoRedirectAfterAuthentication NOTIFY fireAutoRedirectAfterAuthenticationChanged)
	Q_PROPERTY(bool autoUpdateCheck READ isAutoUpdateCheck WRITE setAutoUpdateCheck NOTIFY fireAutoUpdateCheckChanged)
	Q_PROPERTY(bool autoUpdateCheckSetByAdmin READ autoUpdateCheckIsSetByAdmin CONSTANT)
	Q_PROPERTY(bool remindUserToClose READ isRemindUserToClose WRITE setRemindUserToClose NOTIFY fireRemindUserToCloseChanged)
	Q_PROPERTY(bool transportPinReminder READ isTransportPinReminder WRITE setTransportPinReminder NOTIFY fireTransportPinReminderChanged)
	Q_PROPERTY(bool showInAppNotifications READ isShowInAppNotifications WRITE setShowInAppNotifications NOTIFY fireShowInAppNotificationsChanged)
	Q_PROPERTY(governikus::AppUpdateDataModel * appUpdateData READ getAppUpdateData NOTIFY fireAppUpdateDataChanged)
	Q_PROPERTY(QUrl customProxyUrl READ getCustomProxyUrl CONSTANT)
	Q_PROPERTY(bool customProxyAttributesPresent READ isCustomProxyAttributesPresent CONSTANT)
	Q_PROPERTY(bool useCustomProxy READ isUseCustomProxy WRITE setUseCustomProxy NOTIFY fireUseCustomProxyChanged)
	Q_PROPERTY(bool useSystemFont READ isUseSystemFont WRITE setUseSystemFont NOTIFY fireUseSystemFontChanged)
	Q_PROPERTY(bool useAnimations READ isUseAnimations WRITE setUseAnimations NOTIFY fireUseAnimationsChanged)
	Q_PROPERTY(ModeOption userDarkMode READ getDarkMode WRITE setDarkMode NOTIFY fireDarkModeChanged)

	private:
		bool mAdvancedSettings;
		bool mIsStartedByAuth;
		bool mShowBetaTesting;

		SettingsModel();
		~SettingsModel() override = default;

	public:
		enum class ModeOption
		{
			ON,
			OFF,
			AUTO
		};
		Q_ENUM(ModeOption)

		[[nodiscard]] QString getLanguage() const;
		void setLanguage(const QString& pLanguage) const;

		[[nodiscard]] bool isAdvancedSettings() const;
		void setAdvancedSettings(bool pEnabled);

		[[nodiscard]] bool isDeveloperOptions() const;
		void setDeveloperOptions(bool pEnabled) const;

		[[nodiscard]] bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled) const;

		[[nodiscard]] bool useSelfauthenticationTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse) const;

		[[nodiscard]] QString getDeviceName() const;
		void setDeviceName(const QString& name);

		Q_INVOKABLE void removeTrustedCertificate(const QString& pFingerprint) const;

		[[nodiscard]] bool getPinPadMode() const;
		void setPinPadMode(bool pPinPadMode);

		[[nodiscard]] bool getShowAccessRights() const;
		void setShowAccessRights(bool pShowAccessRights);

		[[nodiscard]] bool isVisualPrivacy() const;
		void setVisualPrivacy(bool pVisualPrivacy);

		[[nodiscard]] bool isShuffleScreenKeyboard() const;
		void setShuffleScreenKeyboard(bool pShuffleScreenKeyboard);

		[[nodiscard]] bool isEnableCanAllowed() const;
		void setEnableCanAllowed(bool pEnableCanAllowed);

		[[nodiscard]] bool isSkipRightsOnCanAllowed() const;
		void setSkipRightsOnCanAllowed(bool pSkipRightsOnCanAllowed);

		[[nodiscard]] bool isSimulatorEnabled() const;
		void setSimulatorEnabled(bool pEnabled) const;

		[[nodiscard]] UiModule getStartupModule() const;
		void setStartupModule(UiModule pModule);

		[[nodiscard]] bool isAutoStartAvailable() const;
		[[nodiscard]] bool isAutoStart() const;
		[[nodiscard]] bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pEnabled);

		[[nodiscard]] bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pEnabled);

		[[nodiscard]] bool isAutoRedirectAfterAuthentication() const;
		void setAutoRedirectAfterAuthentication(bool pEnabled);

		[[nodiscard]] bool isAutoUpdateAvailable() const;
		[[nodiscard]] bool isAutoUpdateCheck() const;
		[[nodiscard]] bool autoUpdateCheckIsSetByAdmin() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);
		[[nodiscard]] bool showTrayIcon() const;

		[[nodiscard]] bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		[[nodiscard]] bool isTransportPinReminder() const;
		void setTransportPinReminder(bool pTransportPinReminder);

		[[nodiscard]] bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications) const;

		[[nodiscard]] QUrl getCustomProxyUrl() const;
		[[nodiscard]] bool isCustomProxyAttributesPresent() const;
		[[nodiscard]] bool isUseCustomProxy() const;
		void setUseCustomProxy(bool pUseCustomProxy) const;

		[[nodiscard]] bool isUseSystemFont() const;
		void setUseSystemFont(bool pUseSystemFont) const;

		bool isUseAnimations() const;
		void setUseAnimations(bool pUseAnimations) const;

		[[nodiscard]] ModeOption getDarkMode() const;
		void setDarkMode(ModeOption pMode);

		[[nodiscard]] Q_INVOKABLE bool requestStoreFeedback() const;
		Q_INVOKABLE void hideFutureStoreFeedbackDialogs() const;

		Q_INVOKABLE void updateAppcast() const;

		[[nodiscard]] AppUpdateDataModel* getAppUpdateData() const;

#ifndef QT_NO_DEBUG
		Q_INVOKABLE void resetHideableDialogs() const;
#endif

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireAdvancedSettingsChanged();
		void fireDeveloperOptionsChanged();
		void fireSimulatorChanged();
		void fireDeviceNameChanged();
		void firePinPadModeChanged();
		void fireShowAccessRightsChanged();
		void fireScreenKeyboardChanged();
		void fireCanAllowedChanged();
		void fireStartupModuleChanged();
		void fireAutoStartChanged();
		void fireAutoCloseWindowAfterAuthenticationChanged();
		void fireAutoRedirectAfterAuthenticationChanged();
		void fireAutoUpdateCheckChanged();
		void fireRemindUserToCloseChanged();
		void fireTransportPinReminderChanged();
		void fireAppUpdateDataChanged();
		void fireShowInAppNotificationsChanged();
		void fireUseCustomProxyChanged();
		void fireUseSystemFontChanged();
		void fireUseAnimationsChanged();
		void fireDarkModeChanged();
		void fireShowTrayIconChanged();

};

} // namespace governikus
