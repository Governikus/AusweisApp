/*!
 * \brief Model implementation for the application.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugInInfo.h"

#include "ReaderInfo.h"
#include "WifiInfo.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class WorkflowContext;

class ApplicationModel
	: public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool nfcEnabled READ isNfcEnabled NOTIFY fireNfcEnabledChanged)
	Q_PROPERTY(bool nfcAvailable READ isNfcAvailable CONSTANT)

	Q_PROPERTY(bool bluetoothEnabled READ isBluetoothEnabled WRITE setBluetoothEnabled NOTIFY fireBluetoothEnabledChanged)
	Q_PROPERTY(bool bluetoothResponding READ isBluetoothResponding NOTIFY fireBluetoothRespondingChanged)
	Q_PROPERTY(bool bluetoothAvailable READ isBluetoothAvailable CONSTANT)
	Q_PROPERTY(bool locationPermissionRequired READ locationPermissionRequired NOTIFY fireBluetoothReaderChanged)

	Q_PROPERTY(bool wifiEnabled MEMBER mWifiEnabled NOTIFY fireWifiEnabledChanged)

	Q_PROPERTY(QString currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)
	Q_PROPERTY(bool foundSelectedReader READ foundSelectedReader NOTIFY fireSelectedReaderChanged)

	QSharedPointer<WorkflowContext> mContext;

	void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);
	ReaderManagerPlugInInfo getFirstPlugInInfo(ReaderManagerPlugInType pType) const;

	private:
		WifiInfo mWifiInfo;
		bool mWifiEnabled;
		bool mBluetoothResponding;

	private Q_SLOTS:
		void onWifiEnabledChanged();

	public:
		ApplicationModel(QObject* pParent = nullptr);
		virtual ~ApplicationModel();
		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		bool isNfcAvailable() const;
		bool isNfcEnabled() const;

		bool isBluetoothAvailable() const;
		bool isBluetoothResponding() const;
		bool isBluetoothEnabled() const;
		void setBluetoothEnabled(bool pEnabled);
		bool locationPermissionRequired() const;

		QString getCurrentWorkflow() const;
		bool foundSelectedReader() const;

		Q_INVOKABLE void enableWifi();

	Q_SIGNALS:
		void fireNfcEnabledChanged();

		void fireBluetoothEnabledChanged();
		void fireBluetoothRespondingChanged();
		void fireBluetoothReaderChanged();

		void fireCurrentWorkflowChanged();
		void fireSelectedReaderChanged();

		void fireWifiEnabledChanged();
		void fireCertificateRemoved(QString pDeviceName);
};


} /* namespace governikus */
