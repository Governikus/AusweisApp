/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the application.
 */

#pragma once

#include "Env.h"
#include "ReaderManagerPlugInInfo.h"
#include "WifiInfo.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QStringList>
#include <QTimer>

#ifdef Q_OS_IOS
Q_FORWARD_DECLARE_OBJC_CLASS(VoiceOverObserver);
#endif

class test_UIPlugInQml;


namespace governikus
{

class ApplicationModel
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_UIPlugInQml;

	Q_PROPERTY(QString storeUrl READ getStoreUrl NOTIFY fireStoreUrlChanged)
	Q_PROPERTY(QUrl releaseNotesUrl READ getReleaseNotesUrl CONSTANT)

	Q_PROPERTY(QmlNfcState nfcState READ getNfcState NOTIFY fireNfcStateChanged)
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderPropertiesUpdated)

	Q_PROPERTY(bool wifiEnabled READ isWifiEnabled NOTIFY fireWifiEnabledChanged)

	Q_PROPERTY(Workflow currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)
	Q_PROPERTY(qsizetype availableReader READ getAvailableReader NOTIFY fireAvailableReaderChanged)

	Q_PROPERTY(QString feedback READ getFeedback NOTIFY fireFeedbackChanged)

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	Q_PROPERTY(QUrl customConfigPath READ getCustomConfigPath CONSTANT)
#endif

	private:
		QSharedPointer<WorkflowContext> mContext;
		WifiInfo mWifiInfo;
		bool mWifiEnabled;
		QStringList mFeedback;
		QTimer mFeedbackTimer;
		const int mFeedbackDisplayLength;
		bool mIsAppInForeground;
#ifdef Q_OS_IOS
		struct Private
		{
			Private();
			~Private();
			VoiceOverObserver* const mObserver;
		};
		const QScopedPointer<Private> mPrivate;
#endif

		ApplicationModel();
		~ApplicationModel() override = default;
		void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	private Q_SLOTS:
		void onApplicationStateChanged(Qt::ApplicationState pState);
		void onWifiEnabledChanged();

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		enum class Settings
		{
			SETTING_WIFI,
			SETTING_NETWORK,
			SETTING_NFC,
			SETTING_APP
		};
		Q_ENUM(Settings)

		enum class Workflow
		{
			WORKFLOW_CHANGE_PIN,
			WORKFLOW_SELF_AUTHENTICATION,
			WORKFLOW_AUTHENTICATION,
			WORKFLOW_SMART,
			WORKFLOW_REMOTE_SERVICE,
			WORKFLOW_NONE
		};
		Q_ENUM(Workflow)

		enum class QmlNfcState
		{
			NFC_UNAVAILABLE,
			NFC_DISABLED,
			NFC_INACTIVE,
			NFC_READY
		};
		Q_ENUM(QmlNfcState)

		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		[[nodiscard]] Q_INVOKABLE int randomInt(int pLowerBound, int pUpperBound) const;

		[[nodiscard]] QString getStoreUrl() const;
		[[nodiscard]] QUrl getReleaseNotesUrl() const;

		[[nodiscard]] QmlNfcState getNfcState() const;
		[[nodiscard]] bool isExtendedLengthApdusUnsupported() const;

		[[nodiscard]] bool isWifiEnabled() const;
		[[nodiscard]] Workflow getCurrentWorkflow() const;
		[[nodiscard]] qsizetype getAvailableReader() const;

		[[nodiscard]] QString getFeedback() const;

		[[nodiscard]] Q_INVOKABLE bool isScreenReaderRunning() const;

		[[nodiscard]] Q_INVOKABLE bool isReaderTypeAvailable(ReaderManagerPlugInType pPlugInType) const;

		Q_INVOKABLE void enableWifi()const;

		Q_INVOKABLE void setClipboardText(const QString& pText) const;
		Q_INVOKABLE void showSettings(const Settings& pAction) const;
		Q_INVOKABLE void showFeedback(const QString& pMessage, bool pReplaceExisting = false);
		Q_INVOKABLE void keepScreenOn(bool pActive) const;

		[[nodiscard]] Q_INVOKABLE QStringList getLicenseText() const;
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		[[nodiscard]] Q_INVOKABLE QUrl getCustomConfigPath() const;
		Q_INVOKABLE void saveEmbeddedConfig(const QUrl& pFilename) const;
#endif
		[[nodiscard]] Q_INVOKABLE QString stripHtmlTags(QString pString) const;
#ifdef Q_OS_IOS
		Q_INVOKABLE void showAppStoreRatingDialog();
#endif

	public Q_SLOTS:
		Q_INVOKABLE void onShowNextFeedback();

	Q_SIGNALS:
		void fireStoreUrlChanged();

		void fireNfcStateChanged();
		void fireReaderPropertiesUpdated();

		void fireCurrentWorkflowChanged();
		void fireAvailableReaderChanged();

		void fireWifiEnabledChanged();

		void fireFeedbackChanged();

		void fireApplicationStateChanged(bool pIsAppInForeground);
};


} // namespace governikus
