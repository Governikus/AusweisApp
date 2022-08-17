/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateCleanUpReaderManager.h"

#include "AppSettings.h"
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

	const auto& reader = context->getReaderName();
	if (!reader.isEmpty() && !context->getStatus().isError())
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
	const auto readerManager = Env::getSingleton<ReaderManager>();
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
#ifdef Q_OS_IOS
		readerManager->stopScan(ReaderManagerPlugInType::NFC, status.isError() ? status.toErrorDescription(true) : QString());
#endif
		readerManager->shelve();
	}
	else
	{
		readerManager->stopScanAll(status.isError() ? status.toErrorDescription(true) : QString());
	}
#ifdef Q_OS_IOS
	readerManager->reset(ReaderManagerPlugInType::NFC);
#endif

	Q_EMIT fireContinue();
}
