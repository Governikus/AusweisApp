/*!
 * \brief Model implementation for the application.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "Env.h"
#include "ReaderInfo.h"
#include "ReaderManagerPlugInInfo.h"
#include "WifiInfo.h"

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

	Q_PROPERTY(QString packageName READ getPackageName CONSTANT)
	Q_PROPERTY(QUrl releaseNotesUrl READ getReleaseNotesUrl CONSTANT)

	Q_PROPERTY(bool nfcEnabled READ isNfcEnabled NOTIFY fireNfcEnabledChanged)
	Q_PROPERTY(bool nfcAvailable READ isNfcAvailable CONSTANT)
	Q_PROPERTY(bool nfcRunning READ isNfcRunning WRITE setNfcRunning NOTIFY fireNfcRunningChanged)
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderPropertiesUpdated)

	Q_PROPERTY(bool bluetoothEnabled READ isBluetoothEnabled WRITE setBluetoothEnabled NOTIFY fireBluetoothEnabledChanged)
	Q_PROPERTY(bool bluetoothResponding READ isBluetoothResponding NOTIFY fireBluetoothRespondingChanged)
	Q_PROPERTY(bool bluetoothAvailable READ isBluetoothAvailable CONSTANT)
	Q_PROPERTY(bool locationPermissionRequired READ locationPermissionRequired NOTIFY fireBluetoothReaderChanged)

	Q_PROPERTY(qreal scaleFactor READ getScaleFactor WRITE setScaleFactor NOTIFY fireScaleFactorChanged)
	Q_PROPERTY(bool wifiEnabled READ isWifiEnabled NOTIFY fireWifiEnabledChanged)

	Q_PROPERTY(QString currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)
	Q_PROPERTY(int availableReader READ getAvailableReader NOTIFY fireAvailableReaderChanged)

	Q_PROPERTY(QString feedback READ getFeedback NOTIFY fireFeedbackChanged)

	QSharedPointer<WorkflowContext> mContext;

	void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);
	ReaderManagerPlugInInfo getFirstPlugInInfo(ReaderManagerPlugInType pType) const;

	private:
		constexpr static qreal DEFAULT_SCALE_FACTOR = 0.6;
		qreal mScaleFactor;
		WifiInfo mWifiInfo;
		bool mWifiEnabled;
		bool mBluetoothResponding;
		QStringList mFeedback;
		QTimer mFeedbackTimer;
		const int mFeedbackDisplayLength;
#ifdef Q_OS_IOS
		struct Private
		{
			Private();
			~Private();
			VoiceOverObserver* const mObserver;
		};
		const QScopedPointer<Private> mPrivate;
#endif

	private Q_SLOTS:
		void onWifiEnabledChanged();

	protected:
		ApplicationModel();
		~ApplicationModel() override = default;
		static ApplicationModel& getInstance();

	public:
		enum class Settings
		{
			SETTING_WIFI,
			SETTING_NETWORK,
			SETTING_NFC,
			SETTING_BLUETOOTH
		};
		Q_ENUM(Settings)

		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		QString getPackageName() const;
		QUrl getReleaseNotesUrl() const;

		bool isNfcAvailable() const;
		bool isNfcEnabled() const;
		bool isNfcRunning() const;
		void setNfcRunning(bool pRunning);
		bool isExtendedLengthApdusUnsupported() const;

		bool isBluetoothAvailable() const;
		bool isBluetoothResponding() const;
		bool isBluetoothEnabled() const;
		void setBluetoothEnabled(bool pEnabled);
		bool locationPermissionRequired() const;

		bool isWifiEnabled() const;
		qreal getScaleFactor() const;
		void setScaleFactor(qreal pScaleFactor);

		QString getCurrentWorkflow() const;
		int getAvailableReader() const;

		QString getFeedback() const;

		Q_INVOKABLE bool isScreenReaderRunning() const;

		Q_INVOKABLE void stopNfcScanWithError(const QString& pError) const;

		Q_INVOKABLE bool isReaderTypeAvailable(ReaderManagerPlugInType pPlugInType) const;

		Q_INVOKABLE void enableWifi();

		Q_INVOKABLE void showSettings(const Settings& pAction);
		Q_INVOKABLE void showSettings(const QString& pAction);
		Q_INVOKABLE void showFeedback(const QString& pMessage, bool pReplaceExisting = false);
		Q_INVOKABLE void keepScreenOn(bool pActive);
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		Q_INVOKABLE QString onlineHelpUrl(const QString& pHelpSectionName);
		Q_INVOKABLE void openOnlineHelp(const QString& pHelpSectionName);
#endif
		Q_INVOKABLE QString stripHtmlTags(QString pString) const;
#ifdef Q_OS_IOS
		Q_INVOKABLE void showAppStoreRatingDialog();
#endif

	public Q_SLOTS:
		Q_INVOKABLE void onShowNextFeedback();

	Q_SIGNALS:
		void fireNfcEnabledChanged();
		void fireNfcRunningChanged();
		void fireReaderPropertiesUpdated();

		void fireBluetoothEnabledChanged();
		void fireBluetoothRespondingChanged();
		void fireBluetoothReaderChanged();

		void fireCurrentWorkflowChanged();
		void fireAvailableReaderChanged();

		void fireScaleFactorChanged();
		void fireWifiEnabledChanged();
		void fireCertificateRemoved(const QString& pDeviceName);

		void fireFeedbackChanged();
};


} // namespace governikus
