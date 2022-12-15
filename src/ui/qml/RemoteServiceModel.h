/*!
 * \brief Model implementation for the remote service component
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderManager.h"
#include "RemoteDeviceModel.h"
#include "WorkflowModel.h"
#include "WorkflowRequest.h"
#include "context/IfdServiceContext.h"

#include <QObject>
#include <QQmlEngine>

namespace governikus
{

class RemoteServiceModel
	: public WorkflowModel
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(bool running READ isRunning NOTIFY fireIsRunningChanged)
	Q_PROPERTY(bool isStarting READ isStarting NOTIFY fireIsStartingChanged)
	Q_PROPERTY(bool runnable READ isRunnable NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool canEnableNfc READ isCanEnableNfc NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QString errorMessage READ getErrorMessage NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool isPairing READ isPairing NOTIFY firePskChanged)
	Q_PROPERTY(QByteArray psk READ getPsk NOTIFY firePskChanged)
	Q_PROPERTY(bool connectedToPairedDevice READ isConnectedToPairedDevice NOTIFY fireConnectedChanged)
	Q_PROPERTY(QString connectionInfo READ getConnectionInfo NOTIFY fireConnectionInfoChanged)
	Q_PROPERTY(QString connectedServerDeviceNames READ getConnectedServerDeviceNames NOTIFY fireConnectedServerDeviceNamesChanged)
	Q_PROPERTY(RemoteDeviceModel * availableRemoteDevices READ getAvailableRemoteDevices CONSTANT)
	Q_PROPERTY(RemoteDeviceModel * knownDevices READ getKnownDevices CONSTANT)
	Q_PROPERTY(bool detectRemoteDevices READ detectRemoteDevices WRITE setDetectRemoteDevices NOTIFY fireDetectionChanged)
	Q_PROPERTY(bool enableTransportPinLink READ enableTransportPinLink NOTIFY fireEstablishPaceChannelUpdated)
	Q_PROPERTY(bool remoteReaderVisible READ getRemoteReaderVisible NOTIFY fireRemoteReaderVisibleChanged)
	Q_PROPERTY(bool requiresLocalNetworkPermission MEMBER mRequiresLocalNetworkPermission CONSTANT)

	private:
		QSharedPointer<IfdServiceContext> mContext;
		bool mRunnable;
		bool mIsStarting;
		bool mCanEnableNfc;
		bool mPairingRequested;
		QString mErrorMessage;
		QByteArray mPsk;
		RemoteDeviceModel mAvailableRemoteDevices;
		RemoteDeviceModel mKnownDevices;
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

		QString getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const;

		void setStarting(bool pStarting);

	private Q_SLOTS:
		void onEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus);
		void onConnectionInfoChanged(bool pConnected);
		void onCardConnected(const QSharedPointer<CardConnection>& pConnection);
		void onCardDisconnected(const QSharedPointer<CardConnection>& pConnection);
		void onConnectedDevicesChanged();
		void onEnvironmentChanged();
		void onApplicationStateChanged(const bool pIsAppInForeground);

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		[[nodiscard]] bool isRunning() const;
		Q_INVOKABLE void setRunning(bool pState, bool pEnablePairing = false);
		[[nodiscard]] bool isStarting() const;

		[[nodiscard]] RemoteDeviceModel* getAvailableRemoteDevices();
		[[nodiscard]] RemoteDeviceModel* getKnownDevices();
		void setDetectRemoteDevices(bool pNewStatus);
		[[nodiscard]] bool detectRemoteDevices() const;
		Q_INVOKABLE bool rememberServer(const QString& pDeviceId);
		Q_INVOKABLE void connectToRememberedServer(const QString& pServerPsk);

		void resetRemoteServiceContext(const QSharedPointer<IfdServiceContext>& pContext = QSharedPointer<IfdServiceContext>());
		void setPairing(bool pEnabled);
		[[nodiscard]] bool isPairing();
		[[nodiscard]] bool isConnectedToPairedDevice() const;
		[[nodiscard]] bool enableTransportPinLink() const;
		[[nodiscard]] bool isRunnable() const;
		[[nodiscard]] bool isCanEnableNfc() const;
		[[nodiscard]] QString getErrorMessage() const;
		[[nodiscard]] QByteArray getPsk() const;
		[[nodiscard]] QString getConnectionInfo() const;
		[[nodiscard]] QString getConnectedServerDeviceNames() const;
		[[nodiscard]] bool getRemoteReaderVisible() const;

		[[nodiscard]] Q_INVOKABLE bool pinPadModeOn() const;
		Q_INVOKABLE void forgetDevice(const QString& pId);
		Q_INVOKABLE void cancelPasswordRequest();
		Q_INVOKABLE void changePinLength();

	Q_SIGNALS:
		void fireStartWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireIsStartingChanged();
		void fireIsRunningChanged();
		void fireEnvironmentChanged();
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged();
		void fireServerPskChanged();
		void fireDetectionChanged();
		void firePairingFailed(const QString& pDeviceName, const QString& pErrorMessage);
		void firePairingSuccess(const QString& pDeviceName);
		void firePairingCompleted();
		void fireConnectionInfoChanged();
		void fireConnectedServerDeviceNamesChanged();
		void fireRemoteReaderVisibleChanged();
		void fireEstablishPaceChannelUpdated();
		void fireCertificateRemoved(const QString& pDeviceName);
};


} // namespace governikus
