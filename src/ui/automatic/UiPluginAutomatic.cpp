/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginAutomatic.h"

#include "Env.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateSelectReader.h"

#include <QFile>
#include <QJsonDocument>
#include <QLoggingCategory>
#include <QtGlobal>

Q_DECLARE_LOGGING_CATEGORY(automatic)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

UiPluginAutomatic::UiPluginAutomatic()
	: UiPlugin()
	, mContext()
	, mPrevUsedAsSDK()
	, mPrevUsedDeveloperMode()
{
}


QString UiPluginAutomatic::getEnvVar(const char* pEnv, const QString& pDefault) const
{
	const QByteArrayList prefixes({"AUSWEISAPP_AUTOMATIC_"_ba, "AUSWEISAPP2_AUTOMATIC_"_ba});
	for (const auto& prefix : prefixes)
	{
		const QByteArray& var = prefix + pEnv;
		if (qEnvironmentVariableIsSet(var.constData()))
		{
			return qEnvironmentVariable(var.constData(), pDefault);
		}
	}
	return pDefault;
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

		const auto& developerMode = getEnvVar("DEVELOPERMODE").toLower();
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


std::pair<QString, QVariant> UiPluginAutomatic::getOptionalData(const ReaderInfo& pInfo) const
{
	if (pInfo.getPluginType() == ReaderManagerPluginType::SIMULATOR)
	{
		const auto& getJsonContent = [](const QString& pFile){
					if (pFile.isEmpty())
					{
						return QByteArray();
					}

					if (QFile inputFile(pFile); inputFile.exists() && inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
					{
						qCDebug(automatic) << "Use file as JSON data:" << pFile;
						return inputFile.readAll();
					}

					qCDebug(automatic) << "Use content as JSON data:" << pFile;
					return pFile.toLatin1();
				};

		const auto& simulator = getJsonContent(qEnvironmentVariable("AUSWEISAPP_AUTOMATIC_SIMULATOR", QString()));
		if (!simulator.isEmpty())
		{
			QJsonParseError jsonError {};
			const auto& json = QJsonDocument::fromJson(simulator, &jsonError);

			if (jsonError.error == QJsonParseError::NoError)
			{
				if (json.isObject())
				{
					return {QString(), json.object()};
				}

				return {QStringLiteral("JSON data is not an object"), QVariant()};
			}

			return {jsonError.errorString(), QVariant()};
		}
	}

	return {QString(), QVariant()};
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
				const auto[error, data] = getOptionalData(readerInfo);
				if (error.isEmpty())
				{
					qCDebug(automatic) << "Automatically insert card into:" << readerInfo.getName();
					Env::getSingleton<ReaderManager>()->insert(readerInfo, data);
					mContext->setStateApproved();
					return;
				}

				qCWarning(automatic) << "Cannot get optional data:" << error;
				break;
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
				const auto& pin = getEnvVar("PIN", QStringLiteral("123456"));
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
				const auto& can = getEnvVar("CAN", QString());
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
				const auto& puk = getEnvVar("PUK", QString());
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
