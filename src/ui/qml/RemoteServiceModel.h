/*!
 * \brief Model implementation for the remote service component
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/RemoteServiceContext.h"
#include "Env.h"
#include "ReaderManager.h"
#include "RemoteDeviceModel.h"
#include "WorkflowModel.h"

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
	Q_PROPERTY(RemoteDeviceModel * combinedDevices READ getCombinedDevices CONSTANT)
	Q_PROPERTY(bool detectRemoteDevices READ detectRemoteDevices WRITE setDetectRemoteDevices NOTIFY fireDetectionChanged)
	Q_PROPERTY(bool enableTransportPinLink READ enableTransportPinLink NOTIFY fireEstablishPaceChannelUpdated)
	Q_PROPERTY(bool requestTransportPin READ isRequestTransportPin NOTIFY fireEstablishPaceChannelUpdated)
	Q_PROPERTY(bool remoteReaderVisible READ getRemoteReaderVisible NOTIFY fireRemoteReaderVisibleChanged)
	Q_PROPERTY(bool requiresLocalNetworkPermission MEMBER mRequiresLocalNetworkPermission CONSTANT)

	private:
		QSharedPointer<RemoteServiceContext> mContext;
		bool mRunnable;
		bool mIsStarting;
		bool mCanEnableNfc;
		bool mPairingRequested;
		bool mRequestTransportPin;
		QString mErrorMessage;
		QByteArray mPsk;
		RemoteDeviceModel mAvailableRemoteDevices;
		RemoteDeviceModel mKnownDevices;
		RemoteDeviceModel mCombinedDevices;
		QString mConnectionInfo;
		QString mConnectedServerDeviceNames;
		QSharedPointer<RemoteDeviceListEntry> mRememberedServerEntry;
		const bool mRequiresLocalNetworkPermission;

		RemoteServiceModel();
		~RemoteServiceModel() override = default;

		void onEnvironmentChanged();
		QString getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const;

		void setStarting(bool pStarting);

	private Q_SLOTS:
		void onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus);
		void onConnectionInfoChanged(bool pConnected);
		void onCardConnectionEstablished(const QSharedPointer<CardConnection>& pConnection);
		void onConnectedDevicesChanged();
		void onEstablishPaceChannelUpdated();

	public:
		bool isRunning() const;
		Q_INVOKABLE void setRunning(bool pState, bool pEnablePairing = false);
		bool isStarting() const;

		RemoteDeviceModel* getAvailableRemoteDevices();
		RemoteDeviceModel* getKnownDevices();
		RemoteDeviceModel* getCombinedDevices();
		void setDetectRemoteDevices(bool pNewStatus);
		bool detectRemoteDevices() const;
		Q_INVOKABLE bool rememberServer(const QString& pDeviceId);
		Q_INVOKABLE void connectToRememberedServer(const QString& pServerPsk);

		void resetRemoteServiceContext(const QSharedPointer<RemoteServiceContext>& pContext = QSharedPointer<RemoteServiceContext>());
		void setPairing(bool pEnabled);
		bool isPairing();
		bool isConnectedToPairedDevice() const;
		bool enableTransportPinLink() const;
		bool isRequestTransportPin() const;
		bool isRunnable() const;
		bool isCanEnableNfc() const;
		QString getErrorMessage() const;
		QByteArray getPsk() const;
		QString getConnectionInfo() const;
		QString getConnectedServerDeviceNames() const;
		bool getRemoteReaderVisible() const;

		Q_INVOKABLE bool pinPadModeOn() const;
		Q_INVOKABLE QString getPasswordType() const;
		Q_INVOKABLE void forgetDevice(const QString& pId);
		Q_INVOKABLE void cancelPasswordRequest();
		Q_INVOKABLE void changePinLength();

	Q_SIGNALS:
		void fireStartWorkflow();
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
};


} // namespace governikus
