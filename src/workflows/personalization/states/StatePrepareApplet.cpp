/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StatePrepareApplet.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>
#include <QSignalBlocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StatePrepareApplet::StatePrepareApplet(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer<PersonalizationContext>(pContext)
{
}


void StatePrepareApplet::run()
{
	const auto func = [this] {
				const auto& context = getContext();
				const auto& smartManager = SmartManager::get();
				//: LABEL ANDROID IOS
				setProgress(context->getProgressValue(), tr("Checking Smart-eID status"));

				switch (smartManager->status())
				{
					case EidStatus::INTERNAL_ERROR:
						qCDebug(card_smart) << "getSmartEidStatus() failed";
						return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_Status_Call_Failed);

					case EidStatus::NO_PROVISIONING:
					{
						if (!context->allowSmartEidInstallation())
						{
							qCDebug(card_smart) << "Loop detected: A previous Smart-eID installation seems to be broken.";
							return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_Installation_Loop);
						}

						const int initialProgress = context->getProgressValue();
						const auto& progressHandler = [this, &initialProgress](int pProgress) {
									QMetaObject::invokeMethod(this, [this, pProgress, &initialProgress]{
											//: LABEL ANDROID IOS
											setProgress(pProgress, tr("Installing Smart-eID"), initialProgress);
										}, Qt::QueuedConnection);
								};
						//: LABEL ANDROID IOS
						setProgress(initialProgress, tr("Installing Smart-eID"));
						if (smartManager->installSmart(progressHandler))
						{
							context->smartEidInstallationSuccessfull();
							qCDebug(card_smart) << "Successfully installed Smart-eID";
							return QVariant();
						}

						qCDebug(card_smart) << "Installation of Smart-eID failed";
						return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_Installation_Failed);
					}

					case EidStatus::UNAVAILABLE:
						qCDebug(card_smart) << "Smart-eID is not available on this device";
						return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_Unavailable);

					case EidStatus::PERSONALIZED:
						if (!smartManager->deletePersonalization())
						{
							qCDebug(card_smart) << "Deletion of Smart-eID personalization failed";
							return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_Delete_Personalization_Failed);
						}

						qCDebug(card_smart) << "Successfully deleted the Smart-eID personalization";
						Q_FALLTHROUGH();

					case EidStatus::NO_PERSONALIZATION:
						switch (smartManager->updateInfo())
						{
							case EidUpdateInfo::INTERNAL_ERROR:
								qCDebug(card_smart) << "updateInfo() failed";
								return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_UpdateInfo_Call_Failed);

							case EidUpdateInfo::UPDATE_AVAILABLE:
								qCDebug(card_smart) << "Update available, delete the Smart-eID first";
								break;

							default:
								qCDebug(card_smart) << "No update available";
								return QVariant();
						}

						Q_FALLTHROUGH();

					case EidStatus::APPLET_UNUSABLE:
					{
						const int initialProgress = context->getProgressValue();
						const auto& progressHandler = [this, &initialProgress](int pProgress) {
									QMetaObject::invokeMethod(this, [this, pProgress, &initialProgress]{
											//: LABEL ANDROID IOS
											setProgress(pProgress, tr("Cleaning up old Smart-eID"), initialProgress, 50);
										}, Qt::QueuedConnection);
								};
						//: LABEL ANDROID IOS
						setProgress(initialProgress, tr("Cleaning up old Smart-eID"));
						if (!smartManager->deleteSmart(progressHandler))
						{
							qCDebug(card_smart) << "Deletion of Smart-eID failed";
							return QVariant::fromValue(FailureCode::Reason::Prepare_Applet_Delete_Smart_Failed);
						}

						qCDebug(card_smart) << "Successfully deleted Smart-eID";
						return QVariant();
					}
				}

				Q_UNREACHABLE();
			};

	mConnections += Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StatePrepareApplet::onCommandDone);
}


void StatePrepareApplet::setProgress(int pProgress, const QString& pMessage, int pInitialValue, int pMaxValue) const
{
	auto context = getContext();
	Q_ASSERT(context);
	Q_ASSERT(pMaxValue > pInitialValue);

	// rewrite progress if we combine two progresses from 0-100 in single progress bar.
	const int progress = (pInitialValue > 0 || pMaxValue < 100)
							? pInitialValue + static_cast<int>((pMaxValue - pInitialValue) / 100.0 * pProgress)
							: pProgress;

	context->setProgress(progress, pMessage);
}


void StatePrepareApplet::onCommandDone(const QVariant& pFailure)
{
	if (getContext()->isWorkflowCancelledInState())
	{
		Q_EMIT fireAbort(FailureCode::Reason::Prepare_Applet_User_Cancelled);
		return;
	}

	if (pFailure.isNull())
	{
		Q_EMIT fireContinue();
		return;
	}

	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
	Q_EMIT fireAbort(pFailure.value<FailureCode::Reason>());
}


void StatePrepareApplet::onUserCancelled()
{
	const QSignalBlocker blocker(this);
	AbstractState::onUserCancelled();
}
