/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "StateSelectReaderType.h"


using namespace governikus;


StateSelectReaderType::StateSelectReaderType(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateSelectReaderType::onExit(QEvent* pEvent)
{
	AbstractGenericState::onExit(pEvent);
	getContext()->setReaderType(ReaderManagerPlugInType::UNKNOWN);
}


void StateSelectReaderType::run()
{
	mConnections += connect(getContext().data(), &WorkflowContext::fireReaderTypeChanged, this, &StateSelectReaderType::onReaderTypeChanged);

	onReaderTypeChanged();
}


void StateSelectReaderType::onReaderTypeChanged()
{
	getContext()->setReaderName(QString());

	switch (getContext()->getReaderType())
	{
		case ReaderManagerPlugInType::BLUETOOTH:
			Q_EMIT fireSelectBluetoothReader();
			break;

		case ReaderManagerPlugInType::NFC:
			Q_EMIT fireSelectNfcReader();
			break;

		case ReaderManagerPlugInType::PCSC:
			Q_EMIT fireSelectPcscReader();
			break;

		case ReaderManagerPlugInType::UNKNOWN:
			qInfo() << "ReaderManagerPlugInType not (yet) set to WorkflowContext, waiting...";
			break;
	}
}
