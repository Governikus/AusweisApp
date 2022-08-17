/*!
 * \brief Model implementation for the application.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderInfo.h"
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

namespace governikus
{

class ApplicationModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QString storeUrl READ getStoreUrl NOTIFY fireStoreUrlChanged)
	Q_PROPERTY(QUrl releaseNotesUrl READ getReleaseNotesUrl CONSTANT)

	Q_PROPERTY(QmlNfcState nfcState READ getNfcState NOTIFY fireNfcStateChanged)
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderPropertiesUpdated)

	Q_PROPERTY(qreal scaleFactor READ getScaleFactor WRITE setScaleFactor NOTIFY fireScaleFactorChanged)
	Q_PROPERTY(bool wifiEnabled READ isWifiEnabled NOTIFY fireWifiEnabledChanged)

	Q_PROPERTY(Workflow currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)
	Q_PROPERTY(int availableReader READ getAvailableReader NOTIFY fireAvailableReaderChanged)

	Q_PROPERTY(QString feedback READ getFeedback NOTIFY fireFeedbackChanged)

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	Q_PROPERTY(QUrl customConfigPath READ getCustomConfigPath CONSTANT)
#endif

	private:
		QSharedPointer<WorkflowContext> mContext;
		constexpr static qreal DEFAULT_SCALE_FACTOR = 0.6;
		qreal mScaleFactor;
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
		ReaderManagerPlugInInfo getFirstPlugInInfo(ReaderManagerPlugInType pType) const;

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

		Q_INVOKABLE int randomInt(int pLowerBound, int pUpperBound) const;

		QString getStoreUrl() const;
		QUrl getReleaseNotesUrl() const;

		QmlNfcState getNfcState() const;
		bool isExtendedLengthApdusUnsupported() const;

		bool isWifiEnabled() const;
		qreal getScaleFactor() const;
		void setScaleFactor(qreal pScaleFactor);

		Workflow getCurrentWorkflow() const;
		int getAvailableReader() const;

		QString getFeedback() const;

		Q_INVOKABLE bool isScreenReaderRunning() const;

		Q_INVOKABLE bool isReaderTypeAvailable(ReaderManagerPlugInType pPlugInType) const;

		Q_INVOKABLE void enableWifi();

		Q_INVOKABLE void setClipboardText(const QString& pText) const;
		Q_INVOKABLE void showSettings(const Settings& pAction);
		Q_INVOKABLE void showFeedback(const QString& pMessage, bool pReplaceExisting = false);
		Q_INVOKABLE void keepScreenOn(bool pActive);
		Q_INVOKABLE QStringList getLicenseText() const;
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		Q_INVOKABLE QString onlineHelpUrl(const QString& pHelpSectionName);
		Q_INVOKABLE void openOnlineHelp(const QString& pHelpSectionName);
		Q_INVOKABLE QUrl getCustomConfigPath();
		Q_INVOKABLE void saveEmbeddedConfig(const QUrl& pFilename);
#endif
		Q_INVOKABLE QString stripHtmlTags(QString pString) const;
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

		void fireScaleFactorChanged();
		void fireWifiEnabledChanged();

		void fireFeedbackChanged();

		void fireApplicationStateChanged(bool pIsAppInForeground);
};


} // namespace governikus
