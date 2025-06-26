/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderManagerPluginInfo.h"
#include "SingletonCreator.h"
#include "WifiInfo.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QStringList>
#include <QTimer>
#include <QtQml/qqmlregistration.h>


#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
Q_FORWARD_DECLARE_OBJC_CLASS(VoiceOverObserver);
#endif

class test_UiPluginQml;
class test_ApplicationModel;


namespace governikus
{

class ApplicationModel
	: public QObject
	, public SingletonCreator<ApplicationModel>
{
	Q_OBJECT
	Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_UiPluginQml;
	friend class ::test_ApplicationModel;

	Q_PROPERTY(QString storeUrl READ getStoreUrl NOTIFY fireStoreUrlChanged)
	Q_PROPERTY(QUrl releaseNotesUrl READ getReleaseNotesUrl CONSTANT)

	Q_PROPERTY(NfcState nfcState READ getNfcState NOTIFY fireNfcStateChanged)
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderPropertiesUpdated)

	Q_PROPERTY(bool isSmartSupported READ isSmartSupported CONSTANT)

	Q_PROPERTY(bool wifiEnabled READ isWifiEnabled NOTIFY fireWifiEnabledChanged)

	Q_PROPERTY(Workflow currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)

	// QT_VERSION_CHECK(6, 8, 0) qint64 to qsizetype
	Q_PROPERTY(qint64 availablePcscReader READ getAvailablePcscReader NOTIFY fireAvailableReaderChanged)
	Q_PROPERTY(qint64 availableRemoteReader READ getAvailableRemoteReader NOTIFY fireAvailableReaderChanged)

	Q_PROPERTY(QString feedback READ getFeedback NOTIFY fireFeedbackChanged)
	Q_PROPERTY(int feedbackTimeout READ getFeedbackTimeout CONSTANT)

	Q_PROPERTY(bool isScreenReaderRunning READ isScreenReaderRunning NOTIFY fireScreenReaderRunningChanged)

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	Q_PROPERTY(QUrl customConfigPath READ getCustomConfigPath CONSTANT)
#endif

	private:
		QSharedPointer<WorkflowContext> mContext;
		WifiInfo mWifiInfo;
		bool mWifiEnabled;
		QStringList mFeedback;
		QTimer mFeedbackTimer;
		bool mIsAppInForeground;
#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
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
		void onStatusChanged(const ReaderManagerPluginInfo& pInfo);

		[[nodiscard]] static constexpr int getFeedbackTimeout()
		{
			return 7000;
		}

	private Q_SLOTS:
		void onApplicationStateChanged(Qt::ApplicationState pState);
		void onWifiEnabledChanged();

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		enum class Settings
		{
			WIFI,
			NETWORK,
			NFC,
			APP
		};
		Q_ENUM(Settings)

		enum class Workflow
		{
			CHANGE_PIN,
			SELF_AUTHENTICATION,
			AUTHENTICATION,
			SMART,
			REMOTE_SERVICE,
			NONE
		};
		Q_ENUM(Workflow)

		enum class NfcState
		{
			UNAVAILABLE,
			DISABLED,
			INACTIVE,
			READY
		};
		Q_ENUM(NfcState)

		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		[[nodiscard]] Q_INVOKABLE int randomInt(int pLowerBound, int pUpperBound) const;

		[[nodiscard]] QString getStoreUrl() const;
		[[nodiscard]] QUrl getReleaseNotesUrl() const;

		[[nodiscard]] NfcState getNfcState() const;
		[[nodiscard]] bool isExtendedLengthApdusUnsupported() const;

		[[nodiscard]] bool isSmartSupported() const;

		[[nodiscard]] bool isWifiEnabled() const;
		[[nodiscard]] Workflow getCurrentWorkflow() const;
		[[nodiscard]] qsizetype getAvailablePcscReader() const;
		[[nodiscard]] qsizetype getAvailableRemoteReader() const;

		[[nodiscard]] QString getFeedback() const;

		[[nodiscard]] bool isScreenReaderRunning() const;

		[[nodiscard]] Q_INVOKABLE bool isReaderTypeAvailable(ReaderManagerPluginType pPluginType) const;

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
		Q_INVOKABLE void showAppStoreRatingDialog() const;
		static void notifyScreenReaderChangedThreadSafe();

	public Q_SLOTS:
		Q_INVOKABLE void onShowNextFeedback();

	Q_SIGNALS:
		void fireAppAboutToQuit();

		void fireStoreUrlChanged();

		void fireNfcStateChanged();
		void fireReaderPropertiesUpdated();

		void fireCurrentWorkflowChanged();
		void fireAvailableReaderChanged();

		void fireWifiEnabledChanged();

		void fireFeedbackChanged();

		void fireApplicationStateChanged(bool pIsAppInForeground);

		void fireScreenReaderRunningChanged();
};


} // namespace governikus
