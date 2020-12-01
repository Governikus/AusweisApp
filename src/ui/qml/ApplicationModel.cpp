/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "Env.h"
#include "HelpAction.h"
#include "LanguageLoader.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "RemoteClient.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

#if !defined(Q_OS_WINRT) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include "PdfExporter.h"
#endif

#include <QFile>
#include <QRegularExpression>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(qml)
Q_DECLARE_LOGGING_CATEGORY(feedback)

void ApplicationModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::NFC)
	{
		Q_EMIT fireNfcEnabledChanged();
		Q_EMIT fireNfcRunningChanged();
	}
}


ApplicationModel::ApplicationModel()
	: mContext()
	, mScaleFactor(DEFAULT_SCALE_FACTOR)
	, mWifiInfo()
	, mWifiEnabled(false)
	, mFeedback()
	, mFeedbackTimer()
	, mFeedbackDisplayLength(3500)
#ifdef Q_OS_IOS
	, mPrivate(new Private())
#endif
{
	const auto generalSettings = &Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(generalSettings, &GeneralSettings::fireLanguageChanged, this, &ApplicationModel::fireStoreUrlChanged);

	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &ApplicationModel::fireReaderPropertiesUpdated);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireAvailableReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireAvailableReaderChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &ApplicationModel::onWifiEnabledChanged);

	onWifiEnabledChanged();

	const auto* const remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireCertificateRemoved, this, &ApplicationModel::fireCertificateRemoved);

	mFeedbackTimer.setSingleShot(true);
	connect(&mFeedbackTimer, &QTimer::timeout, this, &ApplicationModel::onShowNextFeedback, Qt::QueuedConnection);
}


void ApplicationModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	if (mContext)
	{
		disconnect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &ApplicationModel::fireAvailableReaderChanged);
	}

	if ((mContext = pContext))
	{
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &ApplicationModel::fireAvailableReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &ApplicationModel::fireAvailableReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderNameChanged, this, &ApplicationModel::fireReaderPropertiesUpdated);
		connect(mContext.data(), &WorkflowContext::fireReaderInfoChanged, this, &ApplicationModel::fireReaderPropertiesUpdated);
	}
	Q_EMIT fireCurrentWorkflowChanged();
}


QString ApplicationModel::getStoreUrl() const
{
	QString languageCode = LanguageLoader::getLocalCode();
	languageCode.replace(QLatin1String("en"), QLatin1String("us"));

#if defined(Q_OS_MACOS)
	return QStringLiteral("https://apps.apple.com/%1/app/ausweisapp2/id948660805").arg(languageCode);

#elif defined(Q_OS_ANDROID)
	return QStringLiteral("market://details?id=%1").arg(BuildHelper::getPackageName());

#elif defined(Q_OS_IOS)
	return QStringLiteral("itms-apps:itunes.apple.com/%1/app/ausweisapp2/id948660805?mt=8&action=write-review").arg(languageCode);

#else
	return QString();

#endif
}


QUrl ApplicationModel::getReleaseNotesUrl() const
{
	const auto storage = Env::getSingleton<SecureStorage>();
	const auto& url = VersionNumber::getApplicationVersion().isDeveloperVersion() ? storage->getAppcastBetaUpdateUrl() : storage->getAppcastUpdateUrl();
	return url.adjusted(QUrl::RemoveFilename).toString() + QStringLiteral("ReleaseNotes.html");
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


bool ApplicationModel::isWifiEnabled() const
{
	return mWifiEnabled;
}


qreal ApplicationModel::getScaleFactor() const
{
	return mScaleFactor;
}


void ApplicationModel::setScaleFactor(qreal pScaleFactor)
{
	pScaleFactor *= DEFAULT_SCALE_FACTOR;

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


int ApplicationModel::getAvailableReader() const
{
	if (!mContext)
	{
		return 0;
	}

	return Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(mContext->getReaderPlugInTypes())).size();
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

QStringList ApplicationModel::getLicenseText() const
{
	QStringList lines;

	QFile licenseFile(QStringLiteral(":/LICENSE.txt"));
	if (!licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCWarning(qml) << "Could not load LICENSE.txt";
		lines << tr("Could not load license text.");
		return lines;
	}

	QTextStream in(&licenseFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	in.setCodec("UTF-8");
#endif
	while (!in.atEnd())
	{
		lines << in.readLine();
	}
	licenseFile.close();

	return lines;
}


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
QString ApplicationModel::onlineHelpUrl(const QString& pHelpSectionName)
{
	return HelpAction::getOnlineUrl(pHelpSectionName);
}


void ApplicationModel::openOnlineHelp(const QString& pHelpSectionName)
{
	HelpAction::openContextHelp(pHelpSectionName);
}


#endif


void ApplicationModel::onWifiEnabledChanged()
{
	mWifiEnabled = mWifiInfo.isWifiEnabled();
	Q_EMIT fireWifiEnabledChanged();
}


void ApplicationModel::enableWifi()
{
#ifdef Q_OS_IOS
	showFeedback(tr("Please enable Wi-Fi in your system settings."));
#elif defined(Q_OS_ANDROID)
	showSettings(Settings::SETTING_WIFI);
#endif
}


QString ApplicationModel::stripHtmlTags(QString pString) const
{
	return pString.remove(QRegularExpression(QStringLiteral("<[^>]*>")));
}
