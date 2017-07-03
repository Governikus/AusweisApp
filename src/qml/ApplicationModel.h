/*!
 * \brief Model implementation for the application.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "ReaderManagerPlugInInfo.h"

#include "ReaderInfo.h"

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
	Q_PROPERTY(bool bluetoothAvailable READ isBluetoothAvailable CONSTANT)
	Q_PROPERTY(bool locationPermissionRequired READ locationPermissionRequired NOTIFY fireBluetoothReaderChanged)
	Q_PROPERTY(QString currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)

	QSharedPointer<WorkflowContext> mContext;

	void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);
	ReaderManagerPlugInInfo getFirstPlugInInfo(ReaderManagerPlugInType pType) const;

	public:
		ApplicationModel(QObject* pParent = nullptr);
		virtual ~ApplicationModel();

		// TODO: remove the singleton when the stationary qml ui used the UIPlugInQml
		static ApplicationModel& getWidgetInstance();

		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());

		bool isNfcAvailable() const;
		bool isNfcEnabled() const;
		bool isBluetoothAvailable() const;
		bool isBluetoothEnabled() const;
		void setBluetoothEnabled(bool pEnabled);
		bool locationPermissionRequired() const;

		QString getCurrentWorkflow() const;

	Q_SIGNALS:
		void fireNfcEnabledChanged();
		void fireBluetoothEnabledChanged();
		void fireCurrentWorkflowChanged();
		void fireBluetoothReaderChanged();
};


} /* namespace governikus */
