/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#include "context/SelfAuthContext.h"

#ifdef Q_OS_ANDROID
	#include "BuildHelper.h"
#endif
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


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)
Q_DECLARE_LOGGING_CATEGORY(feedback)


void ApplicationModel::notifyScreenReaderChangedThreadSafe()
{
	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
				auto* applicationModel = Env::getSingleton<ApplicationModel>();
				if (applicationModel)
				{
					Q_EMIT applicationModel->fireScreenReaderRunningChanged();
				}
			}, Qt::QueuedConnection);
}


void ApplicationModel::onStatusChanged(const ReaderManagerPluginInfo& pInfo)
{
#if defined(QT_NO_DEBUG) || defined(Q_OS_IOS) || defined(Q_OS_ANDROID) || defined(Q_OS_WINRT)

	const ReaderManagerPluginType type = ReaderManagerPluginType::NFC;
#else
	const ReaderManagerPluginType type = ReaderManagerPluginType::PCSC;
#endif

	if (pInfo.getPluginType() == type)
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
	, mIsAppInForeground(true)
#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
	, mPrivate(new Private())
#endif
{
	const auto* readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &ApplicationModel::fireReaderPropertiesUpdated);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(readerManager, &ReaderManager::firePluginAdded, this, &ApplicationModel::onStatusChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireAvailableReaderChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireAvailableReaderChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &ApplicationModel::onWifiEnabledChanged);

	onWifiEnabledChanged();

	mFeedbackTimer.setSingleShot(true);
	connect(&mFeedbackTimer, &QTimer::timeout, this, &ApplicationModel::onShowNextFeedback, Qt::QueuedConnection);

	onApplicationStateChanged(QGuiApplication::applicationState());
	connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, &ApplicationModel::onApplicationStateChanged);
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &ApplicationModel::fireAppAboutToQuit);
}


void ApplicationModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	if (mContext)
	{
		disconnect(mContext.data(), &WorkflowContext::fireReaderPluginTypesChanged, this, &ApplicationModel::fireAvailableReaderChanged);
	}

	if ((mContext = pContext))
	{
		connect(mContext.data(), &WorkflowContext::fireReaderPluginTypesChanged, this, &ApplicationModel::fireAvailableReaderChanged);
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
	const auto* storage = Env::getSingleton<SecureStorage>();
	const auto& url = VersionNumber::getApplicationVersion().isBetaVersion() ? storage->getAppcastBetaUpdateUrl() : storage->getAppcastUpdateUrl();
	return url.adjusted(QUrl::RemoveFilename).toString() + QStringLiteral("ReleaseNotes.html");
}


ApplicationModel::NfcState ApplicationModel::getNfcState() const
{
#if defined(QT_NO_DEBUG) || defined(Q_OS_IOS) || defined(Q_OS_ANDROID) || defined(Q_OS_WINRT)
	const ReaderManagerPluginType type = ReaderManagerPluginType::NFC;
#else
	const ReaderManagerPluginType type = ReaderManagerPluginType::PCSC;
#endif

	const auto& pluginInfo = Env::getSingleton<ReaderManager>()->getPluginInfo(type);

	if (!pluginInfo.isAvailable())
	{
		return NfcState::UNAVAILABLE;
	}

	if (!pluginInfo.isEnabled())
	{
		return NfcState::DISABLED;
	}

	if (!pluginInfo.isScanRunning())
	{
		return NfcState::INACTIVE;
	}

	return NfcState::READY;
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


bool ApplicationModel::isSmartSupported() const
{
#if __has_include("SmartManager.h")
	return true;

#else
	return false;

#endif

}


bool ApplicationModel::isWifiEnabled() const
{
	return mWifiEnabled;
}


ApplicationModel::Workflow ApplicationModel::getCurrentWorkflow() const
{
	if (mContext.objectCast<ChangePinContext>())
	{
		return Workflow::CHANGE_PIN;
	}
	if (mContext.objectCast<SelfAuthContext>())
	{
		return Workflow::SELF_AUTHENTICATION;
	}
#if __has_include("context/PersonalizationContext.h")
	if (mContext.objectCast<PersonalizationContext>())
	{
		return Workflow::SMART;
	}
#endif
	if (mContext.objectCast<AuthContext>())
	{
		return Workflow::AUTHENTICATION;
	}
	if (mContext.objectCast<IfdServiceContext>())
	{
		return Workflow::REMOTE_SERVICE;
	}
	return Workflow::NONE;
}


qsizetype ApplicationModel::getAvailablePcscReader() const
{
	if (!mContext)
	{
		return 0;
	}

	return Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({ReaderManagerPluginType::PCSC})).size();
}


qsizetype ApplicationModel::getAvailableRemoteReader() const
{
	if (!mContext)
	{
		return 0;
	}

	return Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({ReaderManagerPluginType::REMOTE_IFD})).size();
}


bool ApplicationModel::isReaderTypeAvailable(ReaderManagerPluginType pPluginType) const
{
	if (!mContext)
	{
		return false;
	}

	if (!mContext->getReaderName().isEmpty())
	{
		return Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName()).getPluginType() == pPluginType;
	}

	return !Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({pPluginType})).isEmpty();
}


QString ApplicationModel::getFeedback() const
{
	return mFeedback.isEmpty() ? QString() : mFeedback.first();
}


void ApplicationModel::onShowNextFeedback()
{
	mFeedback.removeFirst();
	if (!mFeedback.isEmpty() && !isScreenReaderRunning())
	{
		mFeedbackTimer.start(getFeedbackTimeout());
	}

	Q_EMIT fireFeedbackChanged();
}


void ApplicationModel::setClipboardText(const QString& pText) const
{
	QGuiApplication::clipboard()->setText(pText);
}


#ifndef Q_OS_ANDROID
void ApplicationModel::showFeedback(const QString& pMessage, bool pReplaceExisting)
{
	qCInfo(feedback).noquote() << pMessage;

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
			mFeedbackTimer.start(getFeedbackTimeout());
		}
		Q_EMIT fireFeedbackChanged();
	}
}


#endif


#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
void ApplicationModel::keepScreenOn(bool pActive) const
{
	qCWarning(qml) << "NOT IMPLEMENTED:" << pActive;
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
	return Env::getSingleton<SecureStorage>()->getCustomConfigPath();
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
	showSettings(Settings::WIFI);
#endif
}


QString ApplicationModel::stripHtmlTags(QString pString) const
{
	pString.replace(QRegularExpression(QStringLiteral("(<br>)+")), QStringLiteral(" "));
	return pString.remove(QRegularExpression(QStringLiteral("<[^>]*>")));
}


#ifndef Q_OS_IOS
void ApplicationModel::showAppStoreRatingDialog() const
{
}


#endif
