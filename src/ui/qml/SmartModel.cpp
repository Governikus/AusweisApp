/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SmartModel.h"

#include "ApplicationModel.h"
#include "ReaderManager.h"
#if __has_include("SmartManager.h")
	#include "SmartManager.h"
#endif

#include <QLoggingCategory>

#include <algorithm>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


SmartModel::SmartModel()
	: QObject(nullptr)
	, mStatus(QmlSmartState::SMART_UNAVAILABLE)
	, mCachedCardInfo(CardType::NONE)
	, mProgress(0)
{
	updateStatus();
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &SmartModel::onStatusChanged);
}


void SmartModel::updateStatus()
{
#if __has_include("SmartManager.h")
	if (mStatus != QmlSmartState::SMART_UPDATING_STATUS)
	{
		setStatus(QmlSmartState::SMART_UPDATING_STATUS);

		Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
				return QVariant::fromValue(SmartManager::get()->status());
			}, this, &SmartModel::onUpdateStatusDone);
	}
#endif
}


void SmartModel::updatePinStatus()
{
	const auto& readerManager = Env::getSingleton<ReaderManager>();
	const auto& infos = readerManager->getPlugInInfos();
	const bool smartAvailable = std::any_of(infos.constBegin(), infos.constEnd(), [](const auto& pInfo)
		{
			return pInfo.getPlugInType() == ReaderManagerPlugInType::SMART && pInfo.isAvailable();
		});

	if (smartAvailable)
	{
		setStatus(QmlSmartState::SMART_UPDATING_STATUS);
		connect(readerManager, &ReaderManager::fireStatusChanged, this, &SmartModel::onUpdatePinStatusDone);
		readerManager->startScan(ReaderManagerPlugInType::SMART);
		return;
	}

	setStatus(QmlSmartState::SMART_UNAVAILABLE);
}


void SmartModel::setProgress(int pProgress)
{
	if (pProgress != mProgress)
	{
		mProgress = pProgress;
		Q_EMIT fireProgressChanged();
	}
}


void SmartModel::setStatus(QmlSmartState pNewStatus)
{
	if (mStatus != pNewStatus)
	{
		mStatus = pNewStatus;
		Q_EMIT fireSmartStateChanged();
	}
}


void SmartModel::onDeletePersonalizationDone(const QVariant& pResult)
{
	Env::getSingleton<ApplicationModel>()->showFeedback(
			pResult.value<bool>() ?
			//: LABEL ANDROID IOS
			tr("Delete data was successful.") :
			//: LABEL ANDROID IOS
			tr("Delete data failed."));

	Q_EMIT fireDeletePersonalizationDone();
}


void SmartModel::onDeleteSmartDone(const QVariant& pResult)
{
	Env::getSingleton<ApplicationModel>()->showFeedback(
			pResult.value<bool>() ?
			//: LABEL ANDROID IOS
			tr("Delete Smart-eID was successful.") :
			//: LABEL ANDROID IOS
			tr("Delete Smart-eID failed."));

	Q_EMIT fireDeleteSmartDone();
	setProgress(0);
}


void SmartModel::onUpdateStatusDone(const QVariant& pResult)
{
	QmlSmartState newStatus = QmlSmartState::SMART_UNAVAILABLE;
#if __has_include("SmartManager.h")
	switch (pResult.value<EidStatus>())
	{
		case EidStatus::INTERNAL_ERROR:
		case EidStatus::UNAVAILABLE:
			newStatus = QmlSmartState::SMART_UNAVAILABLE;
			break;

		case EidStatus::NO_PROVISIONING:
			newStatus = QmlSmartState::SMART_NO_PROVISIONING;
			break;

		case EidStatus::NO_PERSONALIZATION:
			newStatus = QmlSmartState::SMART_NO_PERSONALIZATION;
			break;

		case EidStatus::APPLET_UNUSABLE:
			newStatus = QmlSmartState::SMART_UNUSABLE;
			break;

		case EidStatus::PERSONALIZED:
			newStatus = QmlSmartState::SMART_READY;
			break;
	}
#else
	Q_UNUSED(pResult)
#endif

	if (newStatus != mStatus)
	{

		if (newStatus == QmlSmartState::SMART_READY)
		{
			updatePinStatus();
			return;
		}

		mCachedCardInfo = CardInfo(CardType::NONE);
		setStatus(newStatus);
	}
}


int SmartModel::getProgress() const
{
	return mProgress;
}


void SmartModel::onUpdatePinStatusDone(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() != ReaderManagerPlugInType::SMART)
	{
		return;
	}

	const auto& readerManager = Env::getSingleton<ReaderManager>();
	disconnect(readerManager, &ReaderManager::fireStatusChanged, this, &SmartModel::onUpdatePinStatusDone);

	const auto& smartInfos = readerManager->getReaderInfos(ReaderFilter({ReaderManagerPlugInType::SMART}));
	auto newStatus = QmlSmartState::SMART_READY;

	if (smartInfos.size() == 1)
	{
		mCachedCardInfo = smartInfos.constFirst().getCardInfo();
	}
	else
	{
		newStatus = QmlSmartState::SMART_UNUSABLE;
		qCCritical(card_smart) << "Expecting exactly one SmartReader!";
	}

	readerManager->stopScan(ReaderManagerPlugInType::SMART);
	setStatus(newStatus);
}


void SmartModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() != ReaderManagerPlugInType::SMART)
	{
		return;
	}

	Q_EMIT fireScanRunningChanged();
}


SmartModel::QmlSmartState SmartModel::getSmartState() const
{
	if (mStatus == QmlSmartState::SMART_READY && (mCachedCardInfo.isPinInitial() || mCachedCardInfo.getRetryCounter() < 1))
	{
		return QmlSmartState::SMART_UNUSABLE;
	}

	return mStatus;
}


void SmartModel::workflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	if (pContext->hasNextWorkflowPending())
	{
		return;
	}

	if (pContext->getAction() == Action::PERSONALIZATION)
	{
		mStatus = QmlSmartState::SMART_UNAVAILABLE;
		updateStatus();
		return;
	}

	updatePinStatus();
}


void SmartModel::deletePersonalization()
{
#if __has_include("SmartManager.h")
	Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
			return SmartManager::get()->deletePersonalization();
		}, this, &SmartModel::onDeletePersonalizationDone);
#endif

	updateStatus();
}


void SmartModel::deleteSmart()
{
#if __has_include("SmartManager.h")
	const auto& progressHandler = [this](int pProgress){
				QMetaObject::invokeMethod(this, [this, pProgress] {
						setProgress(pProgress);
					}, Qt::QueuedConnection);
			};

	setProgress(0);

	Env::getSingleton<ReaderManager>()->callExecuteCommand([progressHandler] {
			return SmartManager::get()->deleteSmart(progressHandler);
		}, this, &SmartModel::onDeleteSmartDone);
#endif

	updateStatus();
}


MobileEidType SmartModel::getMobileEidType()
{
	return mCachedCardInfo.getMobileEidType();
}


bool SmartModel::isScanRunning() const
{
	return Env::getSingleton<ReaderManager>()->isScanRunning(ReaderManagerPlugInType::SMART);
}
