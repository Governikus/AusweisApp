/*!
 * \brief Model implementation for the settings.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateDataModel.h"
#include "Env.h"

#include <QObject>

namespace governikus
{

class SettingsModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString translationTrigger READ getEmptyString NOTIFY fireLanguageChanged)
	Q_PROPERTY(QString language READ getLanguage WRITE setLanguage NOTIFY fireLanguageChanged)
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
	Q_PROPERTY(bool showSetupAssistantOnStart READ isShowSetupAssistantOnStart WRITE setShowSetupAssistantOnStart NOTIFY fireShowSetupAssistantOnStartChanged)
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
	Q_PROPERTY(bool showNewUiHint READ isShowNewUiHint WRITE setShowNewUiHint NOTIFY fireShowNewUiHintChanged)

	private:
		bool mIsStartedByAuth;
		bool mShowBetaTesting;

	protected:
		SettingsModel();
		~SettingsModel() override = default;
		static SettingsModel& getInstance();

	public:
		QString getEmptyString();
		QString getLanguage() const;
		void setLanguage(const QString& pLanguage);

		bool isDeveloperOptions() const;
		void setDeveloperOptions(bool pEnabled);

		bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		bool useSelfauthenticationTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);

		QString getServerName() const;
		void setServerName(const QString& name);

		Q_INVOKABLE void removeTrustedCertificate(const QString& pFingerprint);
		Q_INVOKABLE int removeHistory(const QString& pPeriodToRemove);
		Q_INVOKABLE int removeEntireHistory();

		bool getPinPadMode() const;
		void setPinPadMode(bool pPinPadMode);

		bool isHistoryEnabled() const;
		void setHistoryEnabled(bool pEnabled);

		bool isUseScreenKeyboard() const;
		void setUseScreenKeyboard(bool pUseScreenKeyboard);

		bool isVisualPrivacy() const;
		void setVisualPrivacy(bool pVisualPrivacy);

		bool isShuffleScreenKeyboard() const;
		void setShuffleScreenKeyboard(bool pShuffleScreenKeyboard);

		bool isShowSetupAssistantOnStart() const;
		void setShowSetupAssistantOnStart(bool pShowSetupAssistantOnStart);

		bool isAutoStartAvailable() const;
		bool isAutoStart() const;
		bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pEnabled);

		bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pEnabled);

		bool isAutoUpdateAvailable() const;
		bool isAutoUpdateCheck() const;
		bool autoUpdateCheckIsSetByAdmin() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);

		bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		bool isTransportPinReminder() const;
		void setTransportPinReminder(bool pTransportPinReminder);

		bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications);

		bool isShowNewUiHint() const;
		void setShowNewUiHint(bool pShowNewUiHint);

		Q_INVOKABLE bool requestStoreFeedback() const;
		Q_INVOKABLE void hideFutureStoreFeedbackDialogs();

		Q_INVOKABLE bool askForDeviceSurvey() const;
		Q_INVOKABLE void setDeviceSurveyPending(bool pDeviceSurveyPending);

		Q_INVOKABLE void updateApp();

		AppUpdateDataModel* getAppUpdateData() const;

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireDeveloperOptionsChanged();
		void fireDeviceNameChanged();
		void firePinPadModeChanged();
		void fireHistoryEnabledChanged();
		void fireScreenKeyboardChanged();
		void fireShowSetupAssistantOnStartChanged();
		void fireAutoStartChanged();
		void fireAutoCloseWindowAfterAuthenticationChanged();
		void fireAutoUpdateCheckChanged();
		void fireRemindUserToCloseChanged();
		void fireTransportPinReminderChanged();
		void fireAppUpdateDataChanged();
		void fireShowInAppNotificationsChanged();
		void fireShowNewUiHintChanged();

};

} // namespace governikus
