/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "RemoteDeviceFilterModel.h"
#include "RemoteDeviceModel.h"
#include "SingletonCreator.h"
#include "WorkflowModel.h"
#include "WorkflowRequest.h"
#include "context/IfdServiceContext.h"

#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>


class test_UiPluginQml;


namespace governikus
{

class RemoteServiceModel
	: public WorkflowModel
	, public SingletonCreator<RemoteServiceModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_UiPluginQml;

	Q_PROPERTY(bool running READ isRunning NOTIFY fireIsRunningChanged)
	Q_PROPERTY(bool isStarting READ isStarting NOTIFY fireIsStartingChanged)
	Q_PROPERTY(bool runnable READ isRunnable NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool canEnableNfc READ isCanEnableNfc NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QString errorMessage READ getErrorMessage NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool isPairing READ isPairing NOTIFY firePskChanged)
	Q_PROPERTY(QByteArray psk READ getPsk NOTIFY firePskChanged)
	Q_PROPERTY(QString displayText READ getDisplayText NOTIFY fireDisplayTextChanged)
	Q_PROPERTY(int percentage READ getPercentage NOTIFY fireDisplayTextChanged)
	Q_PROPERTY(bool connectedToPairedDevice READ isConnectedToPairedDevice NOTIFY fireConnectedChanged)
	Q_PROPERTY(QString connectionInfo READ getConnectionInfo NOTIFY fireConnectionInfoChanged)
	Q_PROPERTY(QString connectedServerDeviceNames READ getConnectedServerDeviceNames NOTIFY fireConnectedServerDeviceNamesChanged)
	Q_PROPERTY(governikus::RemoteDeviceModel * allDevices READ getAllDevices CONSTANT)
	Q_PROPERTY(governikus::RemoteDeviceFilterModel * availableDevicesInPairingMode READ getAvailableDevicesInPairingMode CONSTANT)
	Q_PROPERTY(governikus::RemoteDeviceFilterModel * availablePairedDevices READ getAvailablePairedDevices CONSTANT)
	Q_PROPERTY(governikus::RemoteDeviceFilterModel * unavailablePairedDevices READ getUnavailablePairedDevices CONSTANT)
	Q_PROPERTY(bool enableTransportPinLink READ enableTransportPinLink NOTIFY fireEstablishPaceChannelUpdated)
	Q_PROPERTY(bool remoteReaderVisible READ getRemoteReaderVisible NOTIFY fireRemoteReaderVisibleChanged)
	Q_PROPERTY(bool requiresLocalNetworkPermission MEMBER mRequiresLocalNetworkPermission CONSTANT)
	Q_PROPERTY(QString transactionInfo READ getTransactionInfo NOTIFY fireTransactionInfoChanged)
	Q_PROPERTY(QString connectedClientName READ getConnectedClientName NOTIFY fireConnectionInfoChanged)

	private:
		QSharedPointer<IfdServiceContext> mContext;
		bool mRunnable;
		bool mIsStarting;
		bool mCanEnableNfc;
		bool mPairingRequested;
		QString mErrorMessage;
		QByteArray mPsk;
		RemoteDeviceModel mAllDevices;
		RemoteDeviceFilterModel mAvailableDevicesInPairingMode;
		RemoteDeviceFilterModel mAvailablePairedDevices;
		RemoteDeviceFilterModel mUnavailablePairedDevices;
		QString mConnectionInfo;
		QString mConnectedServerDeviceNames;
		QSharedPointer<IfdListEntry> mRememberedServerEntry;
		const bool mRequiresLocalNetworkPermission;
#if defined(Q_OS_IOS)
		bool mWasRunning;
		bool mWasPairing;
#endif

		RemoteServiceModel();
		~RemoteServiceModel() override = default;

		QString getErrorMessage(bool pNfcPluginEnabled, bool pWifiEnabled) const;
		QRegularExpression getPercentMatcher() const;

		void setStarting(bool pStarting);

	private Q_SLOTS:
		void onEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus);
		void onConnectionInfoChanged(bool pConnected);
		void onCardConnected(const QSharedPointer<CardConnection>& pConnection) const;
		void onCardDisconnected(const QSharedPointer<CardConnection>& pConnection) const;
		void onConnectedDevicesChanged();
		void onEnvironmentChanged();
#ifdef Q_OS_IOS
		void onApplicationStateChanged(const bool pIsAppInForeground);
#else
		void onApplicationStateChanged(const bool pIsAppInForeground) const;
#endif
		void onPairingCompleted(const QSslCertificate& pCertificate);
		void onNameChanged();

	public Q_SLOTS:
		void onTranslationChanged();
		void onReaderPluginTypesChanged(bool pExplicitStart);

	public:
		[[nodiscard]] bool isRunning() const;
		Q_INVOKABLE void setRunning(bool pState, bool pEnablePairing = false);
		[[nodiscard]] bool isStarting() const;

		[[nodiscard]] RemoteDeviceModel* getAllDevices();
		[[nodiscard]] RemoteDeviceFilterModel* getAvailableDevicesInPairingMode();
		[[nodiscard]] RemoteDeviceFilterModel* getAvailablePairedDevices();
		[[nodiscard]] RemoteDeviceFilterModel* getUnavailablePairedDevices();
		Q_INVOKABLE void startDetection();
		Q_INVOKABLE void stopDetection(bool pStopScan);
		Q_INVOKABLE bool rememberServer(const QVariant& pDeviceId);
		Q_INVOKABLE void connectToRememberedServer(const QByteArray& pServerPsk);
		[[nodiscard]] QList<ReaderManagerPluginType> getSupportedReaderPluginTypes() const override;

		void resetRemoteServiceContext(const QSharedPointer<IfdServiceContext>& pContext = QSharedPointer<IfdServiceContext>());
		void setPairing(bool pEnabled) const;
		[[nodiscard]] bool isPairing() const;
		[[nodiscard]] bool isConnectedToPairedDevice() const;
		[[nodiscard]] bool enableTransportPinLink() const;
		[[nodiscard]] bool isRunnable() const;
		[[nodiscard]] bool isCanEnableNfc() const;
		[[nodiscard]] QString getErrorMessage() const;
		[[nodiscard]] QByteArray getPsk() const;
		[[nodiscard]] QString getDisplayText() const;
		[[nodiscard]] int getPercentage() const;
		[[nodiscard]] QString getConnectionInfo() const;
		[[nodiscard]] QString getConnectedServerDeviceNames() const;
		[[nodiscard]] bool getRemoteReaderVisible() const;
		[[nodiscard]] QString getTransactionInfo() const;
		[[nodiscard]] QString getConnectedClientName() const;

		[[nodiscard]] Q_INVOKABLE bool pinPadModeOn() const;
		Q_INVOKABLE void forgetDevice(const QVariant& pId);
		Q_INVOKABLE void cancelPasswordRequest();
		Q_INVOKABLE void passwordsDiffer();
		Q_INVOKABLE void changePinLength();
		[[nodiscard]] Q_INVOKABLE bool isPinAuthentication() const;

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireIsStartingChanged();
		void fireIsRunningChanged();
		void fireEnvironmentChanged();
		void firePskChanged(const QByteArray& pPsk);
		void fireDisplayTextChanged();
		void fireConnectedChanged();
		void fireConnectedServerDisconnected();
		void fireServerPskChanged();
		void firePairingFailed(const QString& pDeviceName, const QString& pErrorMessage);
		void firePairingSuccess(const QString& pDeviceName);
		void firePairingCompleted();
		void fireConnectionInfoChanged();
		void fireConnectedServerDeviceNamesChanged();
		void fireRemoteReaderVisibleChanged();
		void fireEstablishPaceChannelUpdated();
		void fireCertificateRemoved(const QString& pDeviceName);
		void fireTransactionInfoChanged();
};


} // namespace governikus
