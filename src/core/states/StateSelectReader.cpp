/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateSelectReader.h"

#include "FuncUtils.h"
#include "ReaderManager.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


bool StateSelectReader::requiresCard(ReaderManagerPlugInType pPlugInType)
{
	return pPlugInType == ReaderManagerPlugInType::PCSC ||
		   pPlugInType == ReaderManagerPlugInType::REMOTE;
}


StateSelectReader::StateSelectReader(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateSelectReader::run()
{
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderDeviceError, this, &StateSelectReader::onReaderDeviceError);

	onReaderInfoChanged();

	ReaderManager& readerManager = ReaderManager::getInstance();
	const auto& readerPlugInTypes = getContext()->getReaderPlugInTypes();
	for (const auto t : readerPlugInTypes)
	{
		readerManager.startScan(t);
	}
}


void StateSelectReader::onReaderInfoChanged()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	Q_ASSERT(context);

	const QVector<ReaderManagerPlugInType>& plugInTypes = context->getReaderPlugInTypes();
	const auto allReaders = ReaderManager::getInstance().getReaderInfos(plugInTypes);
	const QVector<ReaderInfo> selectableReaders = filter<ReaderInfo>([](const ReaderInfo& info)
			{
				return info.isConnected() && (!requiresCard(info.getPlugInType()) || info.hasEidCard());
			}, allReaders);

	if (selectableReaders.isEmpty())
	{
		qCDebug(statemachine) << "No selectable reader detected";

		return;
	}

	const ReaderInfo& readerInfo = selectableReaders.first();
	const QString readerName = readerInfo.getName();
	context->setReaderName(readerName);
	qCDebug(statemachine) << "Select first found reader:" << readerName;
	qCDebug(statemachine) << "Type:" << readerInfo.getPlugInType() << "BasicReader:" << readerInfo.isBasicReader();

	Q_EMIT fireContinue();
}


void StateSelectReader::onAbort()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	Q_ASSERT(context);

	ReaderManager::getInstance().stopScanAll();

	const auto& readerName = context->getReaderName();
	if (!readerName.isEmpty())
	{
		const ReaderInfo readerInfo = ReaderManager::getInstance().getReaderInfo(readerName);
		if (readerInfo.isConnected())
		{
			ReaderManager::getInstance().disconnectReader(readerName);
		}
	}
	Q_EMIT fireRetry();
}


void StateSelectReader::onReaderDeviceError(DeviceError pDeviceError)
{
	if (pDeviceError != DeviceError::DEVICE_POWERED_OFF && pDeviceError != DeviceError::DEVICE_SCAN_ERROR)
	{
		updateStatus(DeviceErrorUtil::toGlobalStatus(pDeviceError));
		Q_EMIT fireAbort();
	}
}


void StateSelectReader::onEntry(QEvent* pEvent)
{
	const WorkflowContext* const context = getContext().data();
	Q_ASSERT(context);

	mConnections += connect(context, &WorkflowContext::fireAbortCardSelection, this, &StateSelectReader::onAbort);

	/*
	 * Note: the plugin types to be used in this state must be already set in the workflow context before this state is entered.
	 * Changing the plugin types in the context, e.g. from {NFC} to {BLUETOOTH}, causes the state to be left with a fireRetry signal.
	 */
	mConnections += connect(context, &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateSelectReader::fireRetry);

	AbstractGenericState::onEntry(pEvent);
}
