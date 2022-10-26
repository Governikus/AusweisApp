/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#include "BuildHelper.h"
#include "Env.h"
#include "HelpAction.h"
#include "LanguageLoader.h"
#include "ProviderConfiguration.h"
#include "Randomizer.h"
#include "ReaderFilter.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

#if !defined(Q_OS_WINRT) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include "PdfExporter.h"
#endif

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
	, mScaleFactor(DEFAULT_SCALE_FACTOR)
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

	onApplicationStateChanged(qGuiApp->applicationState());
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


ApplicationModel::QmlNfcState ApplicationModel::getNfcState() const
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	const ReaderManagerPlugInType type = ReaderManagerPlugInType::PCSC;
#else
	const ReaderManagerPlugInType type = ReaderManagerPlugInType::NFC;
#endif

	const auto& pluginInfo = getFirstPlugInInfo(type);

	if (!pluginInfo.isAvailable())
	{
		return QmlNfcState::NFC_UNAVAILABLE;
	}

	if (!pluginInfo.isEnabled())
	{
		return QmlNfcState::NFC_DISABLED;
	}

	if (!Env::getSingleton<ReaderManager>()->isScanRunning(type))
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
	if (mContext.objectCast<AuthContext>())
	{
		return Workflow::WORKFLOW_AUTHENTICATION;
	}
#if __has_include("context/PersonalizationContext.h")
	if (mContext.objectCast<PersonalizationContext>())
	{
		return Workflow::WORKFLOW_SMART;
	}
#endif
	return Workflow::WORKFLOW_NONE;
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
	Q_UNUSED(pReplaceExisting)
	QNativeInterface::QAndroidApplication::runOnAndroidMainThread([pMessage](){
			QJniEnvironment env;

			QJniObject context = QNativeInterface::QAndroidApplication::context();
			const QJniObject& jMessage = QJniObject::fromString(pMessage);
			const QJniObject& toast = QJniObject::callStaticObjectMethod(
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
void ApplicationModel::keepScreenOn(bool pActive)
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


QUrl ApplicationModel::getCustomConfigPath()
{
	QFileInfo info(Env::getSingleton<SecureStorage>()->getCustomConfig());
	QUrl url(info.absolutePath());
	url.setScheme(QStringLiteral("file"));
	return url;
}


void ApplicationModel::saveEmbeddedConfig(const QUrl& pFilename)
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


void ApplicationModel::enableWifi()
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
