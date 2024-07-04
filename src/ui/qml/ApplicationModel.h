/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the application.
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

#ifdef Q_OS_IOS
Q_FORWARD_DECLARE_OBJC_CLASS(VoiceOverObserver);
#endif

class test_UiPluginQml;


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

	Q_PROPERTY(QString storeUrl READ getStoreUrl NOTIFY fireStoreUrlChanged)
	Q_PROPERTY(QUrl releaseNotesUrl READ getReleaseNotesUrl CONSTANT)

	Q_PROPERTY(NfcState nfcState READ getNfcState NOTIFY fireNfcStateChanged)
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderPropertiesUpdated)

	Q_PROPERTY(bool isSmartSupported READ isSmartSupported CONSTANT)

	Q_PROPERTY(bool wifiEnabled READ isWifiEnabled NOTIFY fireWifiEnabledChanged)

	Q_PROPERTY(Workflow currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)

	// QT_VERSION_CHECK(6, 8, 0) qint64 to qsizetype
	Q_PROPERTY(qint64 availableReader READ getAvailableReader NOTIFY fireAvailableReaderChanged)
	Q_PROPERTY(qint64 availablePcscReader READ getAvailablePcscReader NOTIFY fireAvailableReaderChanged)

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
		void onStatusChanged(const ReaderManagerPluginInfo& pInfo);

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
		[[nodiscard]] qsizetype getAvailableReader() const;
		[[nodiscard]] qsizetype getAvailablePcscReader() const;

		[[nodiscard]] QString getFeedback() const;

		[[nodiscard]] Q_INVOKABLE bool isScreenReaderRunning() const;

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
