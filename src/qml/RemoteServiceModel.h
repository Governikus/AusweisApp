/*!
 * \brief Model implementation for the remote service component
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/RemoteServiceContext.h"
#include "ReaderManager.h"
#include "RemoteDeviceModel.h"
#include "WifiInfo.h"

#include <QObject>

namespace governikus
{


class RemoteServiceModel
	: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString currentState READ getCurrentState NOTIFY fireCurrentStateChanged)
	Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY fireIsRunningChanged)
	Q_PROPERTY(bool runnable MEMBER mRunnable NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(bool canEnableNfc MEMBER mCanEnableNfc NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QString errorMessage MEMBER mErrorMessage NOTIFY fireEnvironmentChanged)
	Q_PROPERTY(QByteArray psk MEMBER mPsk NOTIFY firePskChanged)
	Q_PROPERTY(QString currentFingerprint READ getCurrentFingerprint NOTIFY fireConnectedChanged)
	Q_PROPERTY(bool connected READ isConnected NOTIFY fireConnectedChanged)
	Q_PROPERTY(QString readerPlugInType READ getReaderPlugInType WRITE setReaderPlugInType NOTIFY fireReaderPlugInTypeChanged)
	Q_PROPERTY(RemoteDeviceModel * availableRemoteDevices READ getAvailableRemoteDevices CONSTANT)
	Q_PROPERTY(RemoteDeviceModel * knownDevices READ getKnownDevices CONSTANT)
	Q_PROPERTY(bool detectRemoteDevices READ detectRemoteDevices WRITE setDetectRemoteDevices NOTIFY fireDetectionChanged)

	private:
		QSharedPointer<RemoteServiceContext> mContext;
		WifiInfo mWifiInfo;
		bool mRunnable;
		bool mCanEnableNfc;
		QString mErrorMessage;
		QByteArray mPsk;
		RemoteDeviceModel mAvailableRemoteDevices;
		RemoteDeviceModel mKnownDevices;

		void onEnvironmentChanged();
		QString getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const;

	private Q_SLOTS:
		void onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus);

	public:
		RemoteServiceModel();

		QString getCurrentState() const;
		bool isRunning() const;
		void setRunning(bool pState);

		QString getReaderPlugInType() const;
		void setReaderPlugInType(const QString& pReaderPlugInType);

		RemoteDeviceModel* getAvailableRemoteDevices();
		RemoteDeviceModel* getKnownDevices();
		void setDetectRemoteDevices(bool pNewStatus);
		bool detectRemoteDevices();
		Q_INVOKABLE void connectToServer(const QString& pDeviceId, const QString& pServerPsk);

		void resetContext(const QSharedPointer<RemoteServiceContext>& pContext = QSharedPointer<RemoteServiceContext>());
		Q_INVOKABLE void setPairing(bool pEnabled = true);
		QString getCurrentFingerprint() const;
		bool isConnected() const;
		Q_INVOKABLE bool pinPadModeOn();
		Q_INVOKABLE QString getPacePasswordId() const;
		Q_INVOKABLE void forgetDevice(const QString& pId);
		Q_INVOKABLE void cancelPasswordRequest();

	Q_SIGNALS:
		void fireStartWorkflow();
		void fireCurrentStateChanged(const QString& pState);
		void fireIsRunningChanged();
		void fireEnvironmentChanged();
		void firePskChanged(const QByteArray& pPsk);
		void fireConnectedChanged(bool pConnected);
		void fireReaderPlugInTypeChanged();
		void fireServerPskChanged();
		void fireDetectionChanged();
		void firePairingFailed();
};


} /* namespace governikus */
