/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "DpiCalculator.h"
#include "Env.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "RemoteClient.h"
#include "SingletonHelper.h"

#if !defined(QT_NO_DEBUG) || defined(Q_OS_ANDROID)
#include "SurveyHandler.h"
#endif

#if (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG)) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#include <QBluetoothLocalDevice>
#endif

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif


using namespace governikus;

defineSingleton(ApplicationModel)


void ApplicationModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::BLUETOOTH)
	{
		Q_EMIT fireBluetoothEnabledChanged();
		Q_EMIT fireBluetoothRespondingChanged();
	}
	else if (pInfo.getPlugInType() == ReaderManagerPlugInType::NFC)
	{
		Q_EMIT fireNfcEnabledChanged();
	}
}


ApplicationModel::ApplicationModel()
	: mContext()
	, mDpiScale(DpiCalculator::getDpiScale())
	, mScaleFactor(mDpiScale)
	, mWifiInfo()
	, mWifiEnabled(false)
	, mBluetoothResponding(true)
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &ApplicationModel::fireReaderPropertiesUpdated);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireSelectedReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireSelectedReaderChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &ApplicationModel::onWifiEnabledChanged);

	onWifiEnabledChanged();

	RemoteClient* const remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireCertificateRemoved, this, &ApplicationModel::fireCertificateRemoved);
}


void ApplicationModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	if (mContext)
	{
		disconnect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &ApplicationModel::fireSelectedReaderChanged);
	}

	if ((mContext = pContext))
	{
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &ApplicationModel::fireSelectedReaderChanged);
	}
	Q_EMIT fireCurrentWorkflowChanged();
}


QString ApplicationModel::getPackageName() const
{
#ifdef Q_OS_ANDROID
	return BuildHelper::getPackageName();

#else
	return QString();

#endif
}


ReaderManagerPlugInInfo ApplicationModel::getFirstPlugInInfo(ReaderManagerPlugInType pType) const
{
	const auto pluginInfos = Env::getSingleton<ReaderManager>()->getPlugInInfos();
	for (const auto& info : pluginInfos)
	{
		if (info.getPlugInType() == pType)
		{
			return info;
		}
	}
	return ReaderManagerPlugInInfo();
}


bool ApplicationModel::isNfcAvailable() const
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	return getFirstPlugInInfo(ReaderManagerPlugInType::PCSC).isAvailable();

#else
	return getFirstPlugInInfo(ReaderManagerPlugInType::NFC).isAvailable();

#endif
}


bool ApplicationModel::isNfcEnabled() const
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	return getFirstPlugInInfo(ReaderManagerPlugInType::PCSC).isEnabled();

#else
	return getFirstPlugInInfo(ReaderManagerPlugInType::NFC).isEnabled();

#endif
}


bool ApplicationModel::isExtendedLengthApdusUnsupported() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		ReaderInfo readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName());
		return !readerInfo.sufficientApduLength();
	}
	return false;
}


bool ApplicationModel::isBluetoothAvailable() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::BLUETOOTH).isAvailable();
}


bool ApplicationModel::isBluetoothResponding() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::BLUETOOTH).isResponding();
}


bool ApplicationModel::isBluetoothEnabled() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::BLUETOOTH).isEnabled();
}


void ApplicationModel::setBluetoothEnabled(bool pEnabled)
{
#if (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG)) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	if (pEnabled)
	{
		QBluetoothLocalDevice localDevice;
		localDevice.powerOn();
		qDebug() << "Bluetooth enabled";
	}
	else
	{
		qWarning() << "Cannot disable Bluetooth: not supported";
	}
#else
	qWarning() << (pEnabled ? "Enabling" : "Disabling") << "Bluetooth not supported on this platform";
#endif
}


bool ApplicationModel::locationPermissionRequired() const
{
#ifdef Q_OS_ANDROID
	const auto& result = QtAndroid::checkPermission(QStringLiteral("android.permission.ACCESS_COARSE_LOCATION"));

	return (result != QtAndroid::PermissionResult::Granted) && Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderManagerPlugInType::BLUETOOTH).isEmpty();

#else
	return false;

#endif
}


qreal ApplicationModel::getScaleFactor() const
{
	return mScaleFactor;
}


void ApplicationModel::setScaleFactor(qreal pScaleFactor)
{
	pScaleFactor *= mDpiScale;

	if (qAbs(pScaleFactor - mScaleFactor) > 0)
	{
		mScaleFactor = pScaleFactor;
		Q_EMIT fireScaleFactorChanged();
	}
}


QString ApplicationModel::getCurrentWorkflow() const
{
	if (mContext.objectCast<ChangePinContext>())
	{
		return QStringLiteral("changepin");
	}
	if (mContext.objectCast<SelfAuthContext>())
	{
		return QStringLiteral("selfauthentication");
	}
	if (mContext.objectCast<AuthContext>())
	{
		return QStringLiteral("authentication");
	}
	return QString();
}


bool ApplicationModel::foundSelectedReader() const
{
	if (!mContext)
	{
		return false;
	}

	return Env::getSingleton<ReaderManager>()->getReaderInfos(mContext->getReaderPlugInTypes()).size() > 0;
}


bool ApplicationModel::areStoreFeedbackDialogConditionsMet() const
{
	if (!getCurrentWorkflow().isEmpty())
	{
		return false;
	}

	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	return !settings.askForDeviceSurvey() && settings.isRequestStoreFeedback();
}


void ApplicationModel::hideFutureStoreFeedbackDialogs()
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setRequestStoreFeedback(false);
}


void ApplicationModel::onWifiEnabledChanged()
{
	mWifiEnabled = mWifiInfo.isWifiEnabled();
	Q_EMIT fireWifiEnabledChanged();
}


void ApplicationModel::enableWifi()
{
	mWifiInfo.enableWifi();
}


ApplicationModel& ApplicationModel::getInstance()
{
	return *Instance;
}
