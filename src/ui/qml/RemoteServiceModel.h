/*!
 * \brief Model implementation for the remote service component
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/RemoteServiceContext.h"
#include "ReaderManager.h"
#include "RemoteDeviceModel.h"
#include "WifiInfo.h"
#include "WorkflowModel.h"

#include <QObject>
#include <QQmlEngine>

class test_RemoteServiceModel;

namespace governikus
{

class RemoteServiceModel
	: public WorkflowModel
{
	Q_OBJECT

	Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY fireIsRunningChanged)
	Q_PROPERTY(bool runnable MEMBER mRunnable NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool canEnableNfc MEMBER mCanEnableNfc NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QString errorMessage MEMBER mErrorMessage NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QByteArray psk MEMBER mPsk NOTIFY firePskChanged)
	Q_PROPERTY(QString currentFingerprint READ getCurrentFingerprint NOTIFY fireConnectedChanged)
	Q_PROPERTY(bool connected READ isConnected NOTIFY fireConnectedChanged)
	Q_PROPERTY(QString connectedClientDeviceName MEMBER mConnectedClientDeviceName NOTIFY fireConnectedClientDeviceNameChanged)
	Q_PROPERTY(QString connectedServerDeviceNames MEMBER mConnectedServerDeviceNames NOTIFY fireConnectedServerDeviceNamesChanged)
	Q_PROPERTY(RemoteDeviceModel * availableRemoteDevices READ getAvailableRemoteDevices CONSTANT)
	Q_PROPERTY(RemoteDeviceModel * knownDevices READ getKnownDevices CONSTANT)
	Q_PROPERTY(bool detectRemoteDevices READ detectRemoteDevices WRITE setDetectRemoteDevices NOTIFY fireDetectionChanged)
	Q_PROPERTY(bool isSaCPinChangeWorkflow READ isSaCPinChangeWorkflow NOTIFY fireEstablishPaceChannelMessageUpdated)

	private:
		friend class ::test_RemoteServiceModel;

		QSharedPointer<RemoteServiceContext> mContext;
		WifiInfo mWifiInfo;
		bool mRunnable;
		bool mCanEnableNfc;
		QString mErrorMessage;
		QByteArray mPsk;
		RemoteDeviceModel mAvailableRemoteDevices;
		RemoteDeviceModel mKnownDevices;
		QString mConnectedClientDeviceName;
		QString mConnectedServerDeviceNames;
		bool mIsSaCPinChangeWorkflow;
		QSharedPointer<RemoteDeviceListEntry> mRememberedServerEntry;

		void onEnvironmentChanged();
		QString getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const;

	private Q_SLOTS:
		void onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus);
		void onClientConnectedChanged(bool pConnected);
		void onConnectedDevicesChanged();
		void onEstablishPaceChannelMessageUpdated(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);

	protected:
		RemoteServiceModel();
		~RemoteServiceModel() override = default;

	public:
		bool isRunning() const;
		void setRunning(bool pState);

		RemoteDeviceModel* getAvailableRemoteDevices();
		RemoteDeviceModel* getKnownDevices();
		void setDetectRemoteDevices(bool pNewStatus);
		bool detectRemoteDevices();
		Q_INVOKABLE bool rememberServer(const QString& pDeviceId);
		Q_INVOKABLE void connectToRememberedServer(const QString& pServerPsk);

		void resetContext(const QSharedPointer<RemoteServiceContext>& pContext = QSharedPointer<RemoteServiceContext>());
		Q_INVOKABLE void setPairing(bool pEnabled = true);
		QString getCurrentFingerprint() const;
		bool isConnected() const;
		bool isSaCPinChangeWorkflow() const;

		Q_INVOKABLE bool pinPadModeOn();
		Q_INVOKABLE QString getPacePasswordId() const;
		Q_INVOKABLE void forgetDevice(const QString& pId);
		Q_INVOKABLE void cancelPasswordRequest();

		static RemoteServiceModel& getInstance();

	Q_SIGNALS:
		void fireIsRunningChanged();
		void fireEnvironmentChanged();
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged(bool pConnected);
		void fireServerPskChanged();
		void fireDetectionChanged();
		void firePairingFailed();
		void fireConnectedClientDeviceNameChanged();
		void fireConnectedServerDeviceNamesChanged();
		void fireEstablishPaceChannelMessageUpdated();
};


} // namespace governikus
