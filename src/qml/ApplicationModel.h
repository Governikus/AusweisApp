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
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderPropertiesUpdated)
	Q_PROPERTY(bool bluetoothEnabled READ isBluetoothEnabled NOTIFY fireBluetoothEnabledChanged)
	Q_PROPERTY(QString currentWorkflow READ getCurrentWorkflow NOTIFY fireCurrentWorkflowChanged)

	QSharedPointer<WorkflowContext> mContext;

	void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);
	bool isEnabled(ReaderManagerPlugInType pType) const;

	public:
		ApplicationModel(QObject* pParent = nullptr);
		virtual ~ApplicationModel();

		void resetContext(QSharedPointer<WorkflowContext> pContext = QSharedPointer<WorkflowContext>());

		bool isNfcEnabled() const;
		bool isExtendedLengthApdusUnsupported() const;
		bool isBluetoothEnabled() const;

		QString getCurrentWorkflow() const;

	Q_SIGNALS:
		void fireNfcEnabledChanged();
		void fireBluetoothEnabledChanged();
		void fireReaderPropertiesUpdated();
		void fireCurrentWorkflowChanged();
};


} /* namespace governikus */
