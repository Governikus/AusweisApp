/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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
	, mStatus(State::UNAVAILABLE)
	, mErrorString()
	, mCachedCardInfo(CardType::NONE)
	, mProgress(0)
{
	updateStatus();
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &SmartModel::onStatusChanged);
}


void SmartModel::updateStatus() NO_SMART_CONST
{
#if __has_include("SmartManager.h")
	const auto* readerManager = Env::getSingleton<ReaderManager>();
	const auto& smartInfo = readerManager->getPluginInfo(ReaderManagerPluginType::SMART);
	if (smartInfo.isAvailable())
	{
		setStatus(State::UPDATING_STATUS);

		Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
					return QVariant::fromValue(SmartManager::get()->status());
				}, this, &SmartModel::onUpdateStatusDone);

		return;
	}

	setStatus(State::UNAVAILABLE);
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
	if (getState() == State::READY || getState() == State::UPDATING_STATUS)
	{
		setStatus(State::UPDATING_STATUS);
		auto* readerManager = Env::getSingleton<ReaderManager>();
		connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &SmartModel::onUpdatePinStatusDone);
		connect(readerManager, &ReaderManager::fireCardInfoChanged, this, &SmartModel::onUpdatePinStatusDone);
		readerManager->startScan(ReaderManagerPluginType::SMART);
		if (Env::getSingleton<ApplicationModel>()->getCurrentWorkflow() == ApplicationModel::Workflow::NONE)
		{
			readerManager->stopScan(ReaderManagerPluginType::SMART);
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


void SmartModel::setStatus(State pNewStatus)
{
	if (mStatus != pNewStatus)
	{
		mStatus = pNewStatus;
		Q_EMIT fireStateChanged();
	}
}


void SmartModel::onUpdateSupportInfoDone(const QVariant& pResult)
{
	State newStatus = State::UNAVAILABLE;
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
				newStatus = State::UNAVAILABLE;
				break;

			case EidSupportStatus::AVAILABLE:
			case EidSupportStatus::UP_TO_DATE:
			case EidSupportStatus::UPDATE_AVAILABLE:
				newStatus = State::NO_PROVISIONING;
				break;
		}
	}
#else
	Q_UNUSED(pResult)
#endif

	if (newStatus != mStatus)
	{
		if (newStatus == State::NO_PROVISIONING)
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


void SmartModel::onDeleteSmartDone(const QVariant& pResult) NO_SMART_CONST
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
	State newStatus = State::UNAVAILABLE;
#if __has_include("SmartManager.h")
	switch (pResult.value<EidStatus>())
	{
		case EidStatus::INTERNAL_ERROR:
			newStatus = State::UNAVAILABLE;
			break;

		case EidStatus::NO_PROVISIONING:
			newStatus = State::NO_PROVISIONING;
			break;

		case EidStatus::NO_PERSONALIZATION:
			newStatus = State::NO_PERSONALIZATION;
			break;

		case EidStatus::UNUSABLE:
		case EidStatus::CERT_EXPIRED:
			newStatus = State::UNUSABLE;
			break;

		case EidStatus::PERSONALIZED:
			newStatus = State::READY;
			break;
	}
#else
	Q_UNUSED(pResult)
#endif

	if (newStatus != mStatus)
	{

		if (newStatus == State::READY)
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
	if (pInfo.getPluginType() != ReaderManagerPluginType::SMART)
	{
		return;
	}

	const auto* readerManager = Env::getSingleton<ReaderManager>();
	disconnect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &SmartModel::onUpdatePinStatusDone);
	disconnect(readerManager, &ReaderManager::fireCardInfoChanged, this, &SmartModel::onUpdatePinStatusDone);

	mCachedCardInfo = pInfo.getCardInfo();

	setStatus(State::READY);
}


void SmartModel::onStatusChanged(const ReaderManagerPluginInfo& pInfo)
{
	if (pInfo.getPluginType() != ReaderManagerPluginType::SMART)
	{
		return;
	}

	if (mStatus == State::UNAVAILABLE && pInfo.isAvailable())
	{
		updateStatus();
	}

	Q_EMIT fireScanRunningChanged();
}


SmartModel::State SmartModel::getState() const
{
	if (mStatus == State::READY && (mCachedCardInfo.isPinInitial() || mCachedCardInfo.getRetryCounter() < 1))
	{
		return State::UNUSABLE;
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
		mStatus = State::UNAVAILABLE;
		updateStatus();
		return;
	}

	updatePinStatus();
}


void SmartModel::updateSupportInfo() NO_SMART_CONST
{
#if __has_include("SmartManager.h")
	if (mStatus != State::UPDATING_STATUS && mStatus != State::READY)
	{
		setErrorString(QString());
		setStatus(State::UPDATING_STATUS);

		Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
					return QVariant::fromValue(SmartManager::get()->updateSupportInfo());
				}, this, &SmartModel::onUpdateSupportInfoDone);
		return;
	}
#endif
}


void SmartModel::deletePersonalization() NO_SMART_CONST
{
#if __has_include("SmartManager.h")
	Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
				return SmartManager::get()->deletePersonalization();
			}, this, &SmartModel::onDeletePersonalizationDone);
#endif

	updateStatus();
}


void SmartModel::deleteSmart() NO_SMART_CONST
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
	return Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::SMART).isScanRunning();
}
