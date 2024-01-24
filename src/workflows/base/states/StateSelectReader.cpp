/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateSelectReader.h"

#include "ReaderManager.h"
#include "VolatileSettings.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateSelectReader::StateSelectReader(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateSelectReader::run()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	*this << connect(readerManager, &ReaderManager::fireReaderAdded, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireCardInserted, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireCardRemoved, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireStatusChanged, this, &StateSelectReader::onReaderStatusChanged);

	onReaderInfoChanged();

	const auto& context = getContext();
	const auto& readerPlugInTypes = Enum<ReaderManagerPlugInType>::getList();
	const auto& enabledPlugInTypes = context->getReaderPlugInTypes();
	for (const auto t : readerPlugInTypes)
	{
		enabledPlugInTypes.contains(t) && !context->skipStartScan() ? readerManager->startScan(t) : readerManager->stopScan(t);
	}
	context->setSkipStartScan(false);
}


void StateSelectReader::onReaderInfoChanged()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	Q_ASSERT(context);
	bool currentReaderHasEidCardButInsufficientApduLength = false;

	const QVector<ReaderManagerPlugInType>& plugInTypes = context->getReaderPlugInTypes();
	const auto allReaders = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(plugInTypes));
	QVector<ReaderInfo> selectableReaders;

	for (const auto& info : allReaders)
	{
		if (info.hasEid())
		{
			if (info.insufficientApduLength())
			{
				currentReaderHasEidCardButInsufficientApduLength = true;
				continue;
			}

			selectableReaders.append(info);
		}
		else if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK()
				&& info.getPlugInType() == ReaderManagerPlugInType::REMOTE_IFD
				&& info.isInsertable())
		{
			Env::getSingleton<ReaderManager>()->insert(info);
			continue;
		}
	}

	context->setCurrentReaderHasEidCardButInsufficientApduLength(currentReaderHasEidCardButInsufficientApduLength);

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


void StateSelectReader::onReaderStatusChanged(const ReaderManagerPlugInInfo& pInfo) const
{
#if defined(Q_OS_IOS)
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK() || pInfo.getPlugInType() != ReaderManagerPlugInType::NFC)
	{
		return;
	}

	if (!pInfo.isScanRunning())
	{
		Q_EMIT getContext()->fireCancelWorkflow();
	}
#else
	Q_UNUSED(pInfo)
#endif
}


void StateSelectReader::onEntry(QEvent* pEvent)
{
	AbstractState::onEntry(pEvent);

	const WorkflowContext* const context = getContext().data();
	Q_ASSERT(context);

	/*
	 * Note: the plugin types to be used in this state must be already set in the workflow context before this state is entered.
	 * Changing the plugin types in the context, e.g. from {NFC} to {REMOTE}, causes the state to be left with a fireRetry signal.
	 */
	*this << connect(context, &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateSelectReader::fireRetry);

}
