/*!
 * \brief Model implementation for the settings.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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
	Q_PROPERTY(bool developerMode READ isDeveloperMode WRITE setDeveloperMode NOTIFY fireDeveloperModeChanged)
	Q_PROPERTY(bool useSelfauthenticationTestUri READ useSelfauthenticationTestUri WRITE setUseSelfauthenticationTestUri NOTIFY fireUseSelfauthenticationTestUriChanged)
	Q_PROPERTY(bool pinPadMode READ getPinPadMode WRITE setPinPadMode NOTIFY firePinPadModeChanged)
	Q_PROPERTY(QString serverName READ getServerName WRITE setServerName NOTIFY fireDeviceNameChanged)
	Q_PROPERTY(bool historyEnabled READ isHistoryEnabled WRITE setHistoryEnabled NOTIFY fireHistoryEnabledChanged)
	Q_PROPERTY(bool useScreenKeyboard READ isUseScreenKeyboard WRITE setUseScreenKeyboard NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool shuffleScreenKeyboard READ isShuffleScreenKeyboard WRITE setShuffleScreenKeyboard NOTIFY fireScreenKeyboardChanged)
	Q_PROPERTY(bool showSetupAssistantOnStart READ isShowSetupAssistantOnStart WRITE setShowSetupAssistantOnStart NOTIFY fireShowSetupAssistantOnStartChanged)
	Q_PROPERTY(bool autoStartApp READ isAutoStart WRITE setAutoStart NOTIFY fireAutoStartChanged)
	Q_PROPERTY(bool autoStartSetByAdmin READ autoStartIsSetByAdmin CONSTANT)
	Q_PROPERTY(bool autoCloseWindowAfterAuthentication READ isAutoCloseWindowAfterAuthentication WRITE setAutoCloseWindowAfterAuthentication NOTIFY fireAutoCloseWindowAfterAuthenticationChanged)
	Q_PROPERTY(bool autoUpdateCheck READ isAutoUpdateCheck WRITE setAutoUpdateCheck NOTIFY fireAutoUpdateCheckChanged)
	Q_PROPERTY(bool autoUpdateCheckSetByAdmin READ autoUpdateCheckIsSetByAdmin CONSTANT)
	Q_PROPERTY(bool remindUserToClose READ isRemindUserToClose WRITE setRemindUserToClose NOTIFY fireRemindUserToCloseChanged)
	Q_PROPERTY(bool showInAppNotifications READ isShowInAppNotifications WRITE setShowInAppNotifications NOTIFY fireShowInAppNotificationsChanged)
	Q_PROPERTY(AppUpdateDataModel * appUpdateData READ getAppUpdateData NOTIFY fireAppUpdateDataChanged)

	private:
		bool mIsStartedByAuth;

	protected:
		SettingsModel();
		~SettingsModel() override = default;
		static SettingsModel& getInstance();

	public:
		QString getEmptyString();
		QString getLanguage() const;
		void setLanguage(const QString& pLanguage);

		bool isDeveloperMode() const;
		void setDeveloperMode(bool pEnabled);

		bool useSelfauthenticationTestUri() const;
		void setUseSelfauthenticationTestUri(bool pUse);

		QString getServerName() const;
		Q_INVOKABLE bool isValidServerName(const QString& name) const;
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

		bool isShuffleScreenKeyboard() const;
		void setShuffleScreenKeyboard(bool pShuffleScreenKeyboard);

		bool isShowSetupAssistantOnStart() const;
		void setShowSetupAssistantOnStart(bool pShowSetupAssistantOnStart);

		bool isAutoStart() const;
		bool autoStartIsSetByAdmin() const;
		void setAutoStart(bool pEnabled);

		bool isAutoCloseWindowAfterAuthentication() const;
		void setAutoCloseWindowAfterAuthentication(bool pEnabled);

		bool isAutoUpdateCheck() const;
		bool autoUpdateCheckIsSetByAdmin() const;
		void setAutoUpdateCheck(bool pAutoUpdateCheck);

		bool isRemindUserToClose() const;
		void setRemindUserToClose(bool pRemindUser);

		bool isShowInAppNotifications() const;
		void setShowInAppNotifications(bool pShowInAppNotifications);

		Q_INVOKABLE bool requestStoreFeedback() const;
		Q_INVOKABLE void hideFutureStoreFeedbackDialogs();

		Q_INVOKABLE bool askForDeviceSurvey() const;
		Q_INVOKABLE void setDeviceSurveyPending(bool pDeviceSurveyPending);

		Q_INVOKABLE void updateApp();

		AppUpdateDataModel* getAppUpdateData() const;

	Q_SIGNALS:
		void fireLanguageChanged();
		void fireDeveloperModeChanged();
		void fireUseSelfauthenticationTestUriChanged();
		void fireDeviceNameChanged();
		void firePinPadModeChanged();
		void fireHistoryEnabledChanged();
		void fireScreenKeyboardChanged();
		void fireShowSetupAssistantOnStartChanged();
		void fireAutoStartChanged();
		void fireAutoCloseWindowAfterAuthenticationChanged();
		void fireAutoUpdateCheckChanged();
		void fireRemindUserToCloseChanged();
		void fireAppUpdateDataChanged(bool pUpdateAvailable, const GlobalStatus& pStatus);
		void fireShowInAppNotificationsChanged();

};

} // namespace governikus
