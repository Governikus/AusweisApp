/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginAutomatic.h"

#include "Env.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateSelectReader.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(automatic)

using namespace governikus;

UiPluginAutomatic::UiPluginAutomatic()
	: UiPlugin()
	, mContext()
	, mPrevUsedAsSDK()
	, mPrevUsedDeveloperMode()
{
}


void UiPluginAutomatic::onApplicationStarted()
{
}


void UiPluginAutomatic::doShutdown()
{
}


void UiPluginAutomatic::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (isDominated())
	{
		return;
	}

	const auto& context = pRequest->getContext();
	context->claim(this);
	if (context.objectCast<AuthContext>())
	{
		qCDebug(automatic) << "Fallback to full automatic UI";

		mContext = context;
		mContext->setReaderPluginTypes({ReaderManagerPluginType::SIMULATOR, ReaderManagerPluginType::PCSC});
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &UiPluginAutomatic::onStateChanged);
		mPrevUsedAsSDK = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();
		mPrevUsedDeveloperMode = Env::getSingleton<VolatileSettings>()->isDeveloperMode();
		Env::getSingleton<VolatileSettings>()->setUsedAsSDK(true);
		Env::getSingleton<ReaderManager>()->startScanAll();

		const auto& developerMode = qEnvironmentVariable("AUSWEISAPP2_AUTOMATIC_DEVELOPERMODE").toLower();
		if (developerMode == QLatin1String("true") || developerMode == QLatin1String("on") || developerMode == QLatin1String("1"))
		{
			Env::getSingleton<VolatileSettings>()->setDeveloperMode(true);
		}
	}
	else
	{
		qCWarning(automatic) << "Cannot handle context... abort automatic workflow";
		context->killWorkflow();
	}
}


void UiPluginAutomatic::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)

	if (isDominated())
	{
		return;
	}

	Env::getSingleton<ReaderManager>()->stopScanAll();
	Env::getSingleton<VolatileSettings>()->setUsedAsSDK(mPrevUsedAsSDK);
	Env::getSingleton<VolatileSettings>()->setDeveloperMode(mPrevUsedDeveloperMode);
	mContext.clear();
}


void UiPluginAutomatic::onStateChanged(const QString& pState)
{
	if (mContext)
	{
		if (StateBuilder::isState<StateSelectReader>(pState))
		{
			handleInsertCard();
		}
		else if (StateBuilder::isState<StateEnterPacePassword>(pState))
		{
			handlePassword();
		}
		else
		{
			mContext->setStateApproved();
		}
	}
}


bool UiPluginAutomatic::isDominated() const
{
	return !mDominator.isNull();
}


void UiPluginAutomatic::handleInsertCard()
{
	const auto* readerManager = Env::getSingleton<ReaderManager>();
	if (readerManager->isInitialScanFinished())
	{
		handleInsertCardScanFinished();
	}
	else
	{
		connect(readerManager, &ReaderManager::fireInitialScanFinished, this, [this]{
				Env::getSingleton<ReaderManager>()->disconnect(this);
				handleInsertCardScanFinished();
			});
	}
}


void UiPluginAutomatic::handleInsertCardScanFinished()
{
	const auto& infos = Env::getSingleton<ReaderManager>()->getReaderInfos();

	if (std::any_of(infos.cbegin(), infos.cend(),
			[](const ReaderInfo& i){return i.hasEid();}))
	{
		qCDebug(automatic) << "Use existing card...";
		mContext->setStateApproved();
		return;
	}
	else
	{
		for (const auto& readerInfo : infos)
		{
			if (readerInfo.isInsertable())
			{
				qCDebug(automatic) << "Automatically insert card into:" << readerInfo.getName();
				Env::getSingleton<ReaderManager>()->insert(readerInfo);
				mContext->setStateApproved();
				return;
			}
		}
	}

	qCWarning(automatic) << "Cannot insert card... abort automatic workflow";
	mContext->killWorkflow();
}


void UiPluginAutomatic::handlePassword()
{
	if (mContext->getLastPaceResult() != CardReturnCode::OK
			&& mContext->getLastPaceResult() != CardReturnCode::OK_PUK
			&& mContext->getLastPaceResult() != CardReturnCode::OK_CAN)
	{
		qCWarning(automatic) << "Previous PACE failed... abort automatic workflow";
		mContext->killWorkflow();
		return;
	}

	const auto& currentReaderInfo = mContext->getCardConnection()->getReaderInfo();
	if (currentReaderInfo.isBasicReader())
	{
		switch (mContext->getEstablishPaceChannelType())
		{
			case PacePasswordId::PACE_PIN:
			{
				const auto& pin = qEnvironmentVariable("AUSWEISAPP2_AUTOMATIC_PIN", QStringLiteral("123456"));
				if (pin.size() == 6)
				{
					mContext->setPin(pin);
					mContext->setStateApproved();
					return;
				}
				break;
			}

			case PacePasswordId::PACE_CAN:
			{
				const auto& can = qEnvironmentVariable("AUSWEISAPP2_AUTOMATIC_CAN", QString());
				if (can.size() == 6)
				{
					mContext->setCan(can);
					mContext->setStateApproved();
					return;
				}
				break;
			}

			case PacePasswordId::PACE_PUK:
			{
				const auto& puk = qEnvironmentVariable("AUSWEISAPP2_AUTOMATIC_PUK", QString());
				if (puk.size() == 10)
				{
					mContext->setPuk(puk);
					mContext->setStateApproved();
					return;
				}
				break;
			}

			case PacePasswordId::UNKNOWN:
			case PacePasswordId::PACE_MRZ:
				break;
		}
	}
	else if (currentReaderInfo.getPluginType() == ReaderManagerPluginType::SIMULATOR)
	{
		mContext->setStateApproved();
		return;
	}

	qCWarning(automatic) << "Cannot handle password... abort automatic workflow";
	mContext->killWorkflow();
}


void UiPluginAutomatic::onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted)
{
	if (pUi == this)
	{
		return;
	}

	if (pAccepted)
	{
		mDominator = pInformation.isEmpty() ? QLatin1String("") : pInformation;
	}
}


void UiPluginAutomatic::onUiDominationReleased()
{
	mDominator.clear();
}
