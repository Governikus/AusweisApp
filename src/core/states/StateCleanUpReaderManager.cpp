/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StateCleanUpReaderManager.h"

#include "AppSettings.h"
#include "context/ChangePinContext.h"
#include "ReaderManager.h"

#include <QDebug>

using namespace governikus;

StateCleanUpReaderManager::StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateCleanUpReaderManager::run()
{
	const QSharedPointer<WorkflowContext> context = getContext();

	// Stop scanning is required to kill all connections to force deleting all pace passwords on a remote comfort reader
	const auto readerManager = Env::getSingleton<ReaderManager>();
	readerManager->stopScanAll();

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	const bool isWidgetUi = generalSettings.getSelectedUi() == QLatin1String("widgets");
	const bool changePinOnWidgets = isWidgetUi && context.objectCast<ChangePinContext>();
	if (changePinOnWidgets || Env::getSingleton<AppSettings>()->isUsedAsSDK())
	{
		readerManager->startScanAll();
	}

	if (context->getCardConnection())
	{
		qDebug() << "Going to disconnect card connection";
		context->resetCardConnection();
	}

	Q_EMIT fireContinue();
}
