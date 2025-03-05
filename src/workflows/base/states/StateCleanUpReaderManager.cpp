/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateCleanUpReaderManager.h"

#include "ReaderManager.h"
#include "VolatileSettings.h"

#include <QDebug>

using namespace governikus;

StateCleanUpReaderManager::StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateCleanUpReaderManager::run()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	if (const auto& reader = context->getReaderName(); !reader.isEmpty())
	{
		const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(reader);
		if (readerInfo.hasCard() && readerInfo.getCardInfo().getCardType() != CardType::SMART_EID)
		{
			context->setRemoveCardFeedback(true);
		}
	}

	if (context->getCardConnection())
	{
		qDebug() << "Going to disconnect card connection";
		context->resetCardConnection();
	}
	context->rememberReader();

	const auto& status = context->getStatus();
	auto* readerManager = Env::getSingleton<ReaderManager>();
	const auto* volatileSettings = Env::getSingleton<VolatileSettings>();
	if (volatileSettings->isUsedAsSDK())
	{
#ifdef Q_OS_IOS
		readerManager->stopScan(ReaderManagerPluginType::NFC, status.isError() ? volatileSettings->getMessages().getSessionFailed() : QString());
#endif
		readerManager->shelve();
	}
	else
	{
		readerManager->stopScanAll(status.isError() ? status.toErrorDescription(true) : QString());
	}
#ifdef Q_OS_IOS
	readerManager->reset(ReaderManagerPluginType::NFC);
#endif

	Q_EMIT fireContinue();
}
