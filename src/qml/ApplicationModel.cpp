/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"
#include "Env.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"

#if (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG)) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#include <QBluetoothLocalDevice>
#endif

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

using namespace governikus;


void ApplicationModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::BLUETOOTH)
	{
		Q_EMIT fireBluetoothEnabledChanged();
	}
	else if (pInfo.getPlugInType() == ReaderManagerPlugInType::NFC)
	{
		Q_EMIT fireNfcEnabledChanged();
	}
}


ApplicationModel::ApplicationModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
	, mWifiInfo()
{
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireStatusChanged, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireSelectedReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireSelectedReaderChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &ApplicationModel::onWifiEnabledChanged);

	onWifiEnabledChanged();

	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
	connect(remoteClient.data(), &RemoteClient::fireCertificateRemoved, this, &ApplicationModel::fireCertificateRemoved);
}


ApplicationModel::~ApplicationModel()
{
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


ReaderManagerPlugInInfo ApplicationModel::getFirstPlugInInfo(ReaderManagerPlugInType pType) const
{
	const auto pluginInfos = ReaderManager::getInstance().getPlugInInfos();
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


bool ApplicationModel::isBluetoothAvailable() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::BLUETOOTH).isAvailable();
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
	if (QtAndroid::androidSdkVersion() < 23)
	{
		return false;
	}

	QAndroidJniEnvironment env;
	const int PERMISSION_GRANTED = 0;
	const auto& permission = QAndroidJniObject::fromString(QStringLiteral("android.permission.ACCESS_COARSE_LOCATION"));

	const auto result = QAndroidJniObject::callStaticMethod<jint>(
			"org/qtproject/qt5/android/QtNative",
			"checkSelfPermission",
			"(Ljava/lang/String;)I",
			permission.object<jstring>());

	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		qWarning() << "Exception occurred while checking location permission";
		return false;
	}

	return (result != PERMISSION_GRANTED) && ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::BLUETOOTH).isEmpty();

#else
	return false;

#endif
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

	return ReaderManager::getInstance().getReaderInfos(mContext->getReaderPlugInTypes()).size() > 0;
}


void ApplicationModel::onWifiEnabledChanged()
{
	mWifiEnabled = mWifiInfo.isWifiEnabled();
	Q_EMIT fireWifiEnabledChanged();
}
