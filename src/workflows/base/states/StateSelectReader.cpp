/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
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
	setHandleNfcStop();
}


void StateSelectReader::run()
{
	auto* readerManager = Env::getSingleton<ReaderManager>();
	*this << connect(readerManager, &ReaderManager::fireReaderAdded, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireCardInserted, this, &StateSelectReader::onReaderInfoChanged);
	*this << connect(readerManager, &ReaderManager::fireCardRemoved, this, &StateSelectReader::onReaderInfoChanged);

	onReaderInfoChanged();

	const auto& context = getContext();
	const auto& readerPluginTypes = Enum<ReaderManagerPluginType>::getList();
	const auto& enabledPluginTypes = context->getReaderPluginTypes();
	for (const auto t : readerPluginTypes)
	{
		enabledPluginTypes.contains(t) ? readerManager->startScan(t) : readerManager->stopScan(t);
	}
}


void StateSelectReader::onReaderInfoChanged()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	Q_ASSERT(context);
	bool currentReaderHasEidCardButInsufficientApduLength = false;

	const QList<ReaderManagerPluginType>& pluginTypes = context->getReaderPluginTypes();
	const auto allReaders = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(pluginTypes));
	QList<ReaderInfo> selectableReaders;

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
				&& info.getPluginType() == ReaderManagerPluginType::REMOTE_IFD
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
	qCDebug(statemachine) << "Type:" << readerInfo.getPluginType() << "BasicReader:" << readerInfo.isBasicReader();

	Q_EMIT fireContinue();
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
	*this << connect(context, &WorkflowContext::fireReaderPluginTypesChanged, this, &StateSelectReader::fireRetry);

}
