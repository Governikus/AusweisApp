/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#include "context/SelfAuthContext.h"

#include "BuildHelper.h"
#include "Env.h"
#include "LanguageLoader.h"
#include "Randomizer.h"
#include "ReaderFilter.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#include <QClipboard>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QRegularExpression>

#ifdef Q_OS_ANDROID
	#include <QJniEnvironment>
	#include <QJniObject>
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)
Q_DECLARE_LOGGING_CATEGORY(feedback)


void ApplicationModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::NFC)
	{
		Q_EMIT fireNfcStateChanged();
	}
}


ApplicationModel::ApplicationModel()
	: mContext()
	, mWifiInfo()
	, mWifiEnabled(false)
	, mFeedback()
	, mFeedbackTimer()
	, mFeedbackDisplayLength(3500)
	, mIsAppInForeground(true)
#ifdef Q_OS_IOS
	, mPrivate(new Private())
#endif
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &ApplicationModel::fireReaderPropertiesUpdated);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireAvailableReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireAvailableReaderChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &ApplicationModel::onWifiEnabledChanged);

	onWifiEnabledChanged();

	mFeedbackTimer.setSingleShot(true);
	connect(&mFeedbackTimer, &QTimer::timeout, this, &ApplicationModel::onShowNextFeedback, Qt::QueuedConnection);

	onApplicationStateChanged(QGuiApplication::applicationState());
	connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &ApplicationModel::onApplicationStateChanged);
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


int ApplicationModel::randomInt(int pLowerBound, int pUpperBound) const
{
	std::uniform_int_distribution<int> distribution(pLowerBound, pUpperBound);
	return distribution(Randomizer::getInstance().getGenerator());
}


QString ApplicationModel::getStoreUrl() const
{
	QString languageCode = LanguageLoader::getLocaleCode();
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
	const auto& url = VersionNumber::getApplicationVersion().isBetaVersion() ? storage->getAppcastBetaUpdateUrl() : storage->getAppcastUpdateUrl();
	return url.adjusted(QUrl::RemoveFilename).toString() + QStringLiteral("ReleaseNotes.html");
}


ApplicationModel::QmlNfcState ApplicationModel::getNfcState() const
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	const ReaderManagerPlugInType type = ReaderManagerPlugInType::PCSC;
#else
	const ReaderManagerPlugInType type = ReaderManagerPlugInType::NFC;
#endif

	const auto& pluginInfo = Env::getSingleton<ReaderManager>()->getPlugInInfo(type);

	if (!pluginInfo.isAvailable())
	{
		return QmlNfcState::NFC_UNAVAILABLE;
	}

	if (!pluginInfo.isEnabled())
	{
		return QmlNfcState::NFC_DISABLED;
	}

	if (!pluginInfo.isScanRunning())
	{
		return QmlNfcState::NFC_INACTIVE;
	}

	return QmlNfcState::NFC_READY;
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
			return readerInfo.insufficientApduLength();
		}
	}
	return false;
}


bool ApplicationModel::isWifiEnabled() const
{
	return mWifiEnabled;
}


ApplicationModel::Workflow ApplicationModel::getCurrentWorkflow() const
{
	if (mContext.objectCast<ChangePinContext>())
	{
		return Workflow::WORKFLOW_CHANGE_PIN;
	}
	if (mContext.objectCast<SelfAuthContext>())
	{
		return Workflow::WORKFLOW_SELF_AUTHENTICATION;
	}
#if __has_include("context/PersonalizationContext.h")
	if (mContext.objectCast<PersonalizationContext>())
	{
		return Workflow::WORKFLOW_SMART;
	}
#endif
	if (mContext.objectCast<AuthContext>())
	{
		return Workflow::WORKFLOW_AUTHENTICATION;
	}
	if (mContext.objectCast<IfdServiceContext>())
	{
		return Workflow::WORKFLOW_REMOTE_SERVICE;
	}
	return Workflow::WORKFLOW_NONE;
}


qsizetype ApplicationModel::getAvailableReader() const
{
	if (!mContext)
	{
		return 0;
	}

	return Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(mContext->getReaderPlugInTypes())).size();
}


qsizetype ApplicationModel::getAvailablePcscReader() const
{
	if (!mContext)
	{
		return 0;
	}

	return Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({ReaderManagerPlugInType::PCSC})).size();
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
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	const jboolean result = context.callMethod<jboolean>("isScreenReaderRunning", "()Z");
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


void ApplicationModel::setClipboardText(const QString& pText) const
{
	QGuiApplication::clipboard()->setText(pText);
}


void ApplicationModel::showFeedback(const QString& pMessage, bool pReplaceExisting)
{
	qCInfo(feedback).noquote() << pMessage;

#if defined(Q_OS_ANDROID)
	QNativeInterface::QAndroidApplication::runOnAndroidMainThread([pMessage, pReplaceExisting](){
			QJniEnvironment env;
			static thread_local QJniObject toast;

			if (toast.isValid() && pReplaceExisting)
			{
				toast.callMethod<void>("cancel");
			}

			QJniObject context = QNativeInterface::QAndroidApplication::context();
			const QJniObject& jMessage = QJniObject::fromString(pMessage);
			toast = QJniObject::callStaticObjectMethod(
					"android/widget/Toast",
					"makeText",
					"(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
					context.object(),
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
void ApplicationModel::keepScreenOn(bool pActive) const
{
	#if defined(Q_OS_ANDROID)
	QNativeInterface::QAndroidApplication::runOnAndroidMainThread([pActive](){
			QJniObject context = QNativeInterface::QAndroidApplication::context();
			context.callMethod<void>("keepScreenOn", "(Z)V", pActive);
			QJniEnvironment env;
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
	while (!in.atEnd())
	{
		lines << in.readLine();
	}
	licenseFile.close();

	return lines;
}


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
QUrl ApplicationModel::getCustomConfigPath() const
{
	QFileInfo info(Env::getSingleton<SecureStorage>()->getCustomConfig());
	QUrl url(info.absolutePath());
	url.setScheme(QStringLiteral("file"));
	return url;
}


void ApplicationModel::saveEmbeddedConfig(const QUrl& pFilename) const
{
	bool success = true;
	if (QFile::exists(pFilename.toLocalFile()))
	{
		success = QFile::remove(pFilename.toLocalFile());
	}
	if (success)
	{
		success = QFile::copy(Env::getSingleton<SecureStorage>()->getEmbeddedConfig(), pFilename.toLocalFile());
	}
	Env::getSingleton<ApplicationModel>()->showFeedback((success ? tr("Successfully saved config to \"%1\"") : tr("Error while saving config to \"%1\"")).arg(pFilename.toLocalFile()));
}


#endif

void ApplicationModel::onApplicationStateChanged(Qt::ApplicationState pState)
{
	const bool isAppInForeground = pState == Qt::ApplicationActive || pState == Qt::ApplicationInactive;
	if (isAppInForeground == mIsAppInForeground)
	{
		return;
	}

	mIsAppInForeground = isAppInForeground;
	Q_EMIT fireApplicationStateChanged(isAppInForeground);
}


void ApplicationModel::onWifiEnabledChanged()
{
	mWifiEnabled = mWifiInfo.isWifiEnabled();
	Q_EMIT fireWifiEnabledChanged();
}


void ApplicationModel::onTranslationChanged()
{
	Q_EMIT fireStoreUrlChanged();
}


void ApplicationModel::enableWifi() const
{
#ifdef Q_OS_ANDROID
	showSettings(Settings::SETTING_WIFI);
#endif
}


QString ApplicationModel::stripHtmlTags(QString pString) const
{
	pString.replace(QRegularExpression(QStringLiteral("(<br>)+")), QStringLiteral(" "));
	return pString.remove(QRegularExpression(QStringLiteral("<[^>]*>")));
}
