/*!
 * \brief Model implementation for the remote service component
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/RemoteServiceContext.h"
#include "Env.h"
#include "ReaderManager.h"
#include "RemoteDeviceModel.h"
#include "WifiInfo.h"
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

	Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY fireIsRunningChanged)
	Q_PROPERTY(bool runnable READ isRunnable NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool canEnableNfc READ isCanEnableNfc NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QString errorMessage READ getErrorMessage NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QByteArray psk READ getPsk NOTIFY firePskChanged)
	Q_PROPERTY(bool connectedToPairedDevice READ isConnectedToPairedDevice NOTIFY fireConnectedChanged)
	Q_PROPERTY(QString connectionInfo READ getConnectionInfo NOTIFY fireConnectionInfoChanged)
	Q_PROPERTY(QString connectedServerDeviceNames READ getConnectedServerDeviceNames NOTIFY fireConnectedServerDeviceNamesChanged)
	Q_PROPERTY(RemoteDeviceModel * availableRemoteDevices READ getAvailableRemoteDevices CONSTANT)
	Q_PROPERTY(RemoteDeviceModel * knownDevices READ getKnownDevices CONSTANT)
	Q_PROPERTY(RemoteDeviceModel * combinedDevices READ getCombinedDevices CONSTANT)
	Q_PROPERTY(bool detectRemoteDevices READ detectRemoteDevices WRITE setDetectRemoteDevices NOTIFY fireDetectionChanged)
	Q_PROPERTY(bool isSaCPinChangeWorkflow READ isSaCPinChangeWorkflow NOTIFY fireEstablishPaceChannelMessageUpdated)

	private:
		QSharedPointer<RemoteServiceContext> mContext;
		WifiInfo mWifiInfo;
		bool mRunnable;
		bool mCanEnableNfc;
		QString mErrorMessage;
		QByteArray mPsk;
		RemoteDeviceModel mAvailableRemoteDevices;
		RemoteDeviceModel mKnownDevices;
		RemoteDeviceModel mCombinedDevices;
		QString mConnectionInfo;
		QString mConnectedServerDeviceNames;
		bool mIsSaCPinChangeWorkflow;
		QSharedPointer<RemoteDeviceListEntry> mRememberedServerEntry;

		void onEnvironmentChanged();
		QString getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const;

	private Q_SLOTS:
		void onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus);
		void onConnectionInfoChanged(bool pConnected);
		void onCardConnectionEstablished(const QSharedPointer<CardConnection>& pConnection);
		void onConnectedDevicesChanged();
		void onEstablishPaceChannelMessageUpdated(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);

	protected:
		RemoteServiceModel();
		~RemoteServiceModel() override = default;
		static RemoteServiceModel& getInstance();

	public:
		bool isRunning() const;
		void setRunning(bool pState);

		RemoteDeviceModel* getAvailableRemoteDevices();
		RemoteDeviceModel* getKnownDevices();
		RemoteDeviceModel* getCombinedDevices();
		void setDetectRemoteDevices(bool pNewStatus);
		bool detectRemoteDevices();
		Q_INVOKABLE bool rememberServer(const QString& pDeviceId);
		Q_INVOKABLE void connectToRememberedServer(const QString& pServerPsk);

		void resetContext(const QSharedPointer<RemoteServiceContext>& pContext = QSharedPointer<RemoteServiceContext>());
		Q_INVOKABLE void setPairing(bool pEnabled = true);
		bool isConnectedToPairedDevice() const;
		bool isSaCPinChangeWorkflow() const;
		bool isRunnable() const;
		bool isCanEnableNfc() const;
		QString getErrorMessage() const;
		QByteArray getPsk() const;
		QString getConnectionInfo() const;
		QString getConnectedServerDeviceNames() const;

		Q_INVOKABLE bool pinPadModeOn();
		Q_INVOKABLE QString getPasswordType() const;
		Q_INVOKABLE void forgetDevice(const QString& pId);
		Q_INVOKABLE void cancelPasswordRequest();

	Q_SIGNALS:
		void fireIsRunningChanged();
		void fireEnvironmentChanged();
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged();
		void fireServerPskChanged();
		void fireDetectionChanged();
		void firePairingFailed(QString pDeviceName, QString pErrorMessage);
		void firePairingSuccess(QString pDeviceName);
		void fireConnectionInfoChanged();
		void fireConnectedServerDeviceNamesChanged();
		void fireEstablishPaceChannelMessageUpdated();
};


} // namespace governikus
