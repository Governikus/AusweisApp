/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
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
	, mErrorString()
	, mCachedCardInfo(CardType::NONE)
	, mProgress(0)
{
	updateStatus();
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &SmartModel::onStatusChanged);
}


void SmartModel::updateStatus()
{
#if __has_include("SmartManager.h")
	const auto& readerManager = Env::getSingleton<ReaderManager>();
	const auto& smartInfo = readerManager->getPlugInInfo(ReaderManagerPlugInType::SMART);
	if (smartInfo.isAvailable())
	{
		setStatus(QmlSmartState::SMART_UPDATING_STATUS);

		Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
				return QVariant::fromValue(SmartManager::get()->status());
			}, this, &SmartModel::onUpdateStatusDone);

		return;
	}

	setStatus(QmlSmartState::SMART_UNAVAILABLE);
#endif
}


void SmartModel::setErrorString(const QString& pError)
{
	if (pError != mErrorString)
	{
		mErrorString = pError;
		Q_EMIT fireErrorStringChanged();
	}
}


void SmartModel::updatePinStatus()
{
	if (getSmartState() == QmlSmartState::SMART_READY || getSmartState() == QmlSmartState::SMART_UPDATING_STATUS)
	{
		setStatus(QmlSmartState::SMART_UPDATING_STATUS);
		const auto& readerManager = Env::getSingleton<ReaderManager>();
		connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &SmartModel::onUpdatePinStatusDone);
		connect(readerManager, &ReaderManager::fireCardInfoChanged, this, &SmartModel::onUpdatePinStatusDone);
		readerManager->startScan(ReaderManagerPlugInType::SMART);
		if (Env::getSingleton<ApplicationModel>()->getCurrentWorkflow() == ApplicationModel::Workflow::WORKFLOW_NONE)
		{
			readerManager->stopScan(ReaderManagerPlugInType::SMART);
		}
		return;
	}
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


void SmartModel::onUpdateSupportInfoDone(const QVariant& pResult)
{
	QmlSmartState newStatus = QmlSmartState::SMART_UNAVAILABLE;
#if __has_include("SmartManager.h")
	const auto& [result, status] = pResult.value<EidSupportStatusResult>();
	switch (result)
	{
		case EidServiceResult::UNDEFINED:
		case EidServiceResult::INFO:
		case EidServiceResult::WARN:
		case EidServiceResult::ERROR:
		case EidServiceResult::UNSUPPORTED:
			//: ERROR ANDROID IOS The check for Smart-eID support failed without any specific reason.
			setErrorString(tr("The online check for the Smart-eID support on your device failed. Please note that this process requires an internet connection."));
			break;

		case EidServiceResult::OVERLOAD_PROTECTION:
			//: ERROR ANDROID IOS The check for Smart-eID support failed because the server is overloaded.
			setErrorString(tr("The online check for the Smart-eID support on your device failed because the server is currently facing too many requests. Please try again later."));
			break;

		case EidServiceResult::UNDER_MAINTENANCE:
			//: ERROR ANDROID IOS The check for Smart-eID support failed because the server is being maintained.
			setErrorString(tr("The online check for the Smart-eID support on your device failed because the server is currently under maintenance. Please try again later."));
			break;

		case EidServiceResult::NFC_NOT_ACTIVATED:
			//: ERROR ANDROID IOS The check for Smart-eID support failed because the NFC functionality is not activated.
			setErrorString(tr("The online check for the Smart-eID support on your device failed. In order to access the necessary device storage, active NFC functionality is required. Please activate NFC and try again."));
			break;

		case EidServiceResult::INTEGRITY_CHECK_FAILED:
			//: ERROR ANDROID IOS The check for Smart-eID support failed because Google Play Integrity Check failed.
			setErrorString(tr("The online check for the Smart-eID support on your device failed. The Google Play Integrity Check failed."));
			return;

		case EidServiceResult::NOT_AUTHENTICATED:
			//: ERROR ANDROID IOS The check for Smart-eID support failed because an authorization issue occurred.
			setErrorString(tr("The online check for the Smart-eID support on your device failed. An authentication issue occurred (e.g. a resource was accessed without authorization or an unauthorized app tried to access a security component)."));
			break;

		case EidServiceResult::NETWORK_CONNECTION_ERROR:
			//: ERROR ANDROID IOS The check for Smart-eID support failed because a network connection error occurred.
			setErrorString(tr("The online check for the Smart-eID support on your device failed. Please ensure that you have an internet connection and your antivirus software and firewall are not blocking the connection."));
			break;

		case EidServiceResult::SUCCESS:
			break;
	}

	if (result == EidServiceResult::SUCCESS)
	{
		switch (status)
		{
			case EidSupportStatus::UNAVAILABLE:
			case EidSupportStatus::INTERNAL_ERROR:
				newStatus = QmlSmartState::SMART_UNAVAILABLE;
				break;

			case EidSupportStatus::AVAILABLE:
			case EidSupportStatus::UP_TO_DATE:
			case EidSupportStatus::UPDATE_AVAILABLE:
				newStatus = QmlSmartState::SMART_NO_PROVISIONING;
				break;
		}
	}
#else
	Q_UNUSED(pResult)
#endif

	if (newStatus != mStatus)
	{
		if (newStatus == QmlSmartState::SMART_NO_PROVISIONING)
		{
			updateStatus();
			return;
		}

		mCachedCardInfo = CardInfo(CardType::NONE);
		setStatus(newStatus);
	}
}


void SmartModel::onDeletePersonalizationDone(const QVariant& pResult)
{
	Q_EMIT fireDeletePersonalizationDone(pResult.value<bool>());
}


void SmartModel::onDeleteSmartDone(const QVariant& pResult)
{
#if __has_include("SmartManager.h")
	switch (pResult.value<EidServiceResult>())
	{
		case EidServiceResult::UNDEFINED:
		case EidServiceResult::INFO:
		case EidServiceResult::WARN:
		case EidServiceResult::ERROR:
		case EidServiceResult::UNSUPPORTED:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed without a specific reason.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device. Please note that this process requires an internet connection."));
			break;

		case EidServiceResult::OVERLOAD_PROTECTION:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed because the server is overloaded.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device because the server is currently facing too many requests. Please try again later."));
			break;

		case EidServiceResult::UNDER_MAINTENANCE:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed because the server is being maintained.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device because the server is currently under maintenance. Please try again later."));
			break;

		case EidServiceResult::NFC_NOT_ACTIVATED:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed because NFC is not activated.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device. In order to access the necessary device storage, active NFC functionality is required. Please activate NFC and restart the process."));
			break;

		case EidServiceResult::INTEGRITY_CHECK_FAILED:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed because the Google Play Integrity Check failed.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device. The Google Play Integrity Check failed."));
			break;

		case EidServiceResult::NOT_AUTHENTICATED:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed because an authorization issue occurred.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device. An authentication issue occurred (e.g. a resource was accessed without authorization or an unauthorized app tried to access a security component)."));
			break;

		case EidServiceResult::NETWORK_CONNECTION_ERROR:
			//: ERROR ANDROID IOS Deletion of the Smart-eID failed because a network connection error occurred.
			setErrorString(tr("The Smart-eID data and provisioning could not be successfully deleted from your device. Please ensure that you have an internet connection and your antivirus software and firewall are not blocking the connection."));
			break;

		case EidServiceResult::SUCCESS:
			break;

	}

	Q_EMIT fireDeleteSmartDone();
	setProgress(0);
#else
	Q_UNUSED(pResult)
#endif
}


void SmartModel::onUpdateStatusDone(const QVariant& pResult)
{
	QmlSmartState newStatus = QmlSmartState::SMART_UNAVAILABLE;
#if __has_include("SmartManager.h")
	switch (pResult.value<EidStatus>())
	{
		case EidStatus::INTERNAL_ERROR:
			newStatus = QmlSmartState::SMART_UNAVAILABLE;
			break;

		case EidStatus::NO_PROVISIONING:
			newStatus = QmlSmartState::SMART_NO_PROVISIONING;
			break;

		case EidStatus::NO_PERSONALIZATION:
			newStatus = QmlSmartState::SMART_NO_PERSONALIZATION;
			break;

		case EidStatus::UNUSABLE:
		case EidStatus::CERT_EXPIRED:
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


void SmartModel::onUpdatePinStatusDone(const ReaderInfo& pInfo)
{
	if (pInfo.getPlugInType() != ReaderManagerPlugInType::SMART)
	{
		return;
	}

	const auto& readerManager = Env::getSingleton<ReaderManager>();
	disconnect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &SmartModel::onUpdatePinStatusDone);
	disconnect(readerManager, &ReaderManager::fireCardInfoChanged, this, &SmartModel::onUpdatePinStatusDone);

	mCachedCardInfo = pInfo.getCardInfo();

	setStatus(QmlSmartState::SMART_READY);
}


void SmartModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() != ReaderManagerPlugInType::SMART)
	{
		return;
	}

	if (mStatus == QmlSmartState::SMART_UNAVAILABLE && pInfo.isAvailable())
	{
		updateStatus();
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


QString SmartModel::getErrorString() const
{
	return mErrorString;
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


void SmartModel::updateSupportInfo()
{
#if __has_include("SmartManager.h")
	if (mStatus != QmlSmartState::SMART_UPDATING_STATUS && mStatus != QmlSmartState::SMART_READY)
	{
		setErrorString(QString());
		setStatus(QmlSmartState::SMART_UPDATING_STATUS);

		Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
				return QVariant::fromValue(SmartManager::get()->updateSupportInfo());
			}, this, &SmartModel::onUpdateSupportInfoDone);
		return;
	}
#endif
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
	setErrorString(QString());

	Env::getSingleton<ReaderManager>()->callExecuteCommand([progressHandler] {
			return QVariant::fromValue(SmartManager::get()->deleteSmart(progressHandler));
		}, this, &SmartModel::onDeleteSmartDone);
#endif

	updateStatus();
}


MobileEidType SmartModel::getMobileEidType() const
{
	return mCachedCardInfo.getMobileEidType();
}


bool SmartModel::isScanRunning() const
{
	return Env::getSingleton<ReaderManager>()->getPlugInInfo(ReaderManagerPlugInType::SMART).isScanRunning();
}
