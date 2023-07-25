/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateEnterPacePassword.h"

#include "ReaderManager.h"
#include "VolatileSettings.h"


using namespace governikus;


StateEnterPacePassword::StateEnterPacePassword(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	const auto& readerManager = Env::getSingleton<ReaderManager>();
	mConnections += connect(readerManager, &ReaderManager::fireStatusChanged, this, &StateEnterPacePassword::onReaderStatusChanged);

	setKeepCardConnectionAlive();
}


void StateEnterPacePassword::run()
{
	if (getContext()->getStatus().isError())
	{
		Q_ASSERT(getContext()->getFailureCode().has_value());
		Q_EMIT firePropagateAbort();
	}
	else
	{
		Q_EMIT fireContinue();
	}
}


void StateEnterPacePassword::onEntry(QEvent* pEvent)
{
	switch (getContext()->getLastPaceResult())
	{
		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
			stopNfcScanIfNecessary();
			break;

		default:
			const auto* volatileSettings = Env::getSingleton<VolatileSettings>();
			//: INFO IOS The current session was interrupted because of a wrong password.
			stopNfcScanIfNecessary(volatileSettings->isUsedAsSDK() ? volatileSettings->getMessages().getSessionFailed() : tr("Access denied."));
	}

	AbstractState::onEntry(pEvent);
}


void StateEnterPacePassword::onReaderStatusChanged(const ReaderManagerPlugInInfo& pInfo) const
{
#if defined(Q_OS_IOS)
	const auto& volatileSettings = Env::getSingleton<VolatileSettings>();
	if (!volatileSettings->isUsedAsSDK() || pInfo.getPlugInType() != ReaderManagerPlugInType::NFC)
	{
		return;
	}

	if (!Env::getSingleton<ReaderManager>()->isScanRunning(ReaderManagerPlugInType::NFC))
	{
		if (getContext()->interruptRequested())
		{
			getContext()->setInterruptRequested(false);
		}
		else
		{
			Q_EMIT getContext()->fireCancelWorkflow();
		}
	}
#else
	Q_UNUSED(pInfo)
#endif
}
