/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "DpiCalculator.h"
#include "Env.h"
#include "HelpAction.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "RemoteClient.h"
#include "SingletonHelper.h"

#if !defined(Q_OS_WINRT) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include "PdfExporter.h"
#endif

#include <QRegularExpression>

#if (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG)) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#include <QBluetoothLocalDevice>
#endif

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QOperatingSystemVersion>
#include <QtAndroid>
#endif


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(qml)
Q_DECLARE_LOGGING_CATEGORY(feedback)

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
		Q_EMIT fireNfcRunningChanged();
	}
}


ApplicationModel::ApplicationModel()
	: mContext()
	, mDpiScale(DpiCalculator::getDpiScale())
	, mScaleFactor(mDpiScale)
	, mWifiInfo()
	, mWifiEnabled(false)
	, mBluetoothResponding(true)
	, mFeedback()
	, mFeedbackTimer()
	, mFeedbackDisplayLength(3500)
#ifdef Q_OS_IOS
	, mPrivate(new Private)
#endif
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

	auto* const remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireCertificateRemoved, this, &ApplicationModel::fireCertificateRemoved);

	mFeedbackTimer.setSingleShot(true);
	connect(&mFeedbackTimer, &QTimer::timeout, this, &ApplicationModel::onShowNextFeedback, Qt::QueuedConnection);
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
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &ApplicationModel::fireSelectedReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &ApplicationModel::fireReaderPropertiesUpdated);
		connect(mContext.data(), &WorkflowContext::fireReaderInfoChanged, this, &ApplicationModel::fireReaderPropertiesUpdated);
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


bool ApplicationModel::isNfcRunning() const
{
	return Env::getSingleton<ReaderManager>()->isScanRunning(ReaderManagerPlugInType::NFC);
}


void ApplicationModel::setNfcRunning(bool pRunning)
{
	const auto& readerManager = Env::getSingleton<ReaderManager>();
	if (pRunning)
	{
		readerManager->startScan(ReaderManagerPlugInType::NFC);
		return;
	}

	readerManager->stopScan(ReaderManagerPlugInType::NFC);
}


bool ApplicationModel::isExtendedLengthApdusUnsupported() const
{

	if (mContext)
	{
		if (mContext->currentReaderHasEidCardButInsufficientApduLength())
		{
			return true;
		}
		if (!mContext->getReaderName().isEmpty())
		{
			ReaderInfo readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName());
			return !readerInfo.sufficientApduLength();
		}
	}
	return false;
}


void ApplicationModel::stopNfcScanWithError(const QString& pError) const
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	readerManager->stopScan(ReaderManagerPlugInType::NFC, pError);
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


bool ApplicationModel::isWifiEnabled() const
{
	return mWifiEnabled;
}


qreal ApplicationModel::getDpiScale() const
{
	return mDpiScale;
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

	return !Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(mContext->getReaderPlugInTypes())).isEmpty();
}


bool ApplicationModel::isReaderTypeAvailable(ReaderManagerPlugInType pPlugInType) const
{
	if (!mContext)
	{
		return false;
	}

	if (!mContext->getReaderName().isEmpty())
	{
		return Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName()).getPlugInType() == pPlugInType;
	}

	return !Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({pPlugInType})).isEmpty();
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction)
{
#ifdef Q_OS_ANDROID
	const auto& androidQ = QOperatingSystemVersion(QOperatingSystemVersion::Android, 10);

	switch (pAction)
	{
		case Settings::SETTING_NETWORK:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSettings(QStringLiteral("android.settings.panel.action.INTERNET_CONNECTIVITY"));
			}
			else
			{
				showSettings(QStringLiteral("android.settings.WIRELESS_SETTINGS"));
			}
			break;

		case Settings::SETTING_NFC:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSettings(QStringLiteral("android.settings.panel.action.NFC"));
			}
			else
			{
				showSettings(QStringLiteral("android.settings.NFC_SETTINGS"));
			}

			break;

		case Settings::SETTING_BLUETOOTH:
			showSettings(QStringLiteral("android.settings.BLUETOOTH_SETTINGS"));
			break;
	}
#else
	qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
#endif
}


void ApplicationModel::showSettings(const QString& pAction)
{
#ifdef Q_OS_ANDROID
	QAndroidJniEnvironment env;

	const QAndroidJniObject& jAction = QAndroidJniObject::fromString(pAction);
	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", jAction.object<jstring>());
	const jint flag = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_NEW_TASK");
	intent.callObjectMethod("setFlags", "(I)V", flag);

	if (intent.isValid())
	{
		qCCritical(qml) << "Call action:" << pAction;
		QtAndroid::startActivity(intent, 0);
	}

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call an action as activity:" << pAction;
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
#else
	qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
#endif
}


#ifndef Q_OS_IOS
bool ApplicationModel::isScreenReaderRunning() const
{
#ifdef Q_OS_ANDROID
	const jboolean result = QtAndroid::androidActivity().callMethod<jboolean>("isScreenReaderRunning", "()Z");
	return result != JNI_FALSE;

#else
	qCWarning(qml) << "NOT IMPLEMENTED";
	return false;

#endif
}


#endif

QString ApplicationModel::getFeedback() const
{
	return mFeedback.isEmpty() ? QString() : mFeedback.first();
}


void ApplicationModel::onShowNextFeedback()
{
	mFeedback.removeFirst();
	if (!mFeedback.isEmpty() && !isScreenReaderRunning())
	{
		mFeedbackTimer.start(mFeedbackDisplayLength);
	}

	Q_EMIT fireFeedbackChanged();
}


void ApplicationModel::showFeedback(const QString& pMessage, bool pReplaceExisting)
{
	qCInfo(feedback).noquote() << pMessage;

#if defined(Q_OS_ANDROID)
	Q_UNUSED(pReplaceExisting)
	// Wait for toast activation synchronously so that the app can not be deactivated
	// in the meantime and all used Java objects are still alive when accessed.
	QtAndroid::runOnAndroidThreadSync([pMessage](){
				QAndroidJniEnvironment env;

				const QAndroidJniObject& jMessage = QAndroidJniObject::fromString(pMessage);
				const QAndroidJniObject& toast = QAndroidJniObject::callStaticObjectMethod(
						"android/widget/Toast",
						"makeText",
						"(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
						QtAndroid::androidActivity().object(),
						jMessage.object(),
						jint(1));
				toast.callMethod<void>("show");

				if (env->ExceptionCheck())
				{
					qCCritical(qml) << "Suppressing an unexpected exception.";
					env->ExceptionDescribe();
					env->ExceptionClear();
				    // The toast was probably not displayed (e.g. DeadObjectException). We halt on error
				    // since it is used to display information to the user as required by the TR.
					Q_ASSERT(false);
				}
			});
#else
	if (pReplaceExisting)
	{
		mFeedbackTimer.stop();
		mFeedback.clear();
	}

	const bool initial = mFeedback.isEmpty();
	mFeedback << pMessage;
	if (initial)
	{
		if (!isScreenReaderRunning())
		{
			mFeedbackTimer.start(mFeedbackDisplayLength);
		}
		Q_EMIT fireFeedbackChanged();
	}
#endif
}


#ifndef Q_OS_IOS
void ApplicationModel::keepScreenOn(bool pActive)
{
#if defined(Q_OS_ANDROID)
	QtAndroid::runOnAndroidThread([pActive](){
				QtAndroid::androidActivity().callMethod<void>("keepScreenOn", "(Z)V", pActive);
				QAndroidJniEnvironment env;
				if (env->ExceptionCheck())
				{
					qCCritical(qml) << "Exception calling java native function.";
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			});

#else
	qCWarning(qml) << "NOT IMPLEMENTED:" << pActive;
#endif
}


#endif


void ApplicationModel::openOnlineHelp(const QString& pHelpSectionName)
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	qCWarning(qml) << "NOT IMPLEMENTED:" << pHelpSectionName;
#else
	HelpAction::openContextHelp(pHelpSectionName);
#endif
}


void ApplicationModel::onWifiEnabledChanged()
{
	mWifiEnabled = mWifiInfo.isWifiEnabled();
	Q_EMIT fireWifiEnabledChanged();
}


void ApplicationModel::enableWifi()
{
#ifdef Q_OS_IOS
	showFeedback(tr("Please enable Wi-Fi in your system settings."));
#else
	mWifiInfo.enableWifi();
#endif
}


ApplicationModel& ApplicationModel::getInstance()
{
	return *Instance;
}


QString ApplicationModel::stripHtmlTags(QString pString) const
{
	return pString.remove(QRegularExpression(QStringLiteral("<[^>]*>")));
}
