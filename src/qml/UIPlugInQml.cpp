/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthenticationContext.h"
#include "DpiCalculator.h"
#include "FileDestination.h"
#include "UIPlugInQml.h"
#include "Updater.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#endif

#include <QFile>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QScreen>
#include <QtPlugin>
#include <QtQml>

#if defined(Q_OS_ANDROID)
#include <QFont>
#endif

#if defined(Q_OS_IOS)
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtQmlModelsPlugin)
Q_IMPORT_PLUGIN(QtQmlStateMachinePlugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPlugin)
//Q_IMPORT_PLUGIN(QtQuickControls2Plugin)
//Q_IMPORT_PLUGIN(QtQuickExtrasFlatPlugin)
//Q_IMPORT_PLUGIN(QtQuickTemplates2Plugin)

#endif

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

UIPlugInQml::UIPlugInQml()
	: mEngine()
	, mDpi(DpiCalculator::getDpi())
	, mProviderModel(&AppSettings::getInstance().getProviderSettings())
	, mHistoryModel(&AppSettings::getInstance().getHistorySettings(), &AppSettings::getInstance().getProviderSettings())
	, mChangePinModel()
	, mAuthModel()
	, mVersionInformationModel()
	, mQmlExtension()
	, mSelfAuthenticationModel()
	, mSettingsModel()
	, mCertificateDescriptionModel()
	, mChatModel()
	, mNumberModel()
	, mApplicationModel()
	, mExplicitPlatformStyle(getPlatformSelectors())
	, mStatusBarUtil()
	, mConnectivityManager()
{
#if defined(Q_OS_ANDROID)
	QGuiApplication::setFont(QFont("Roboto"));
#endif

	connect(&mChangePinModel, &ChangePinModel::fireStartWorkflow, this, &UIPlugIn::fireChangePinRequest);
	connect(&mSelfAuthenticationModel, &SelfAuthenticationModel::fireStartWorkflow, this, &UIPlugIn::fireSelfAuthenticationRequested);
	init();
}


UIPlugInQml::~UIPlugInQml()
{
}


void UIPlugInQml::init()
{
#ifndef QT_NO_DEBUG
	// Our ios implementation is placed in the base folder.
	// When setting on android extra selectors to ios, the ios (i.e. the base) implementations
	// are not found. This happens because the selector ordering in QFileSelector is
	//    1. extra selectors
	//    2. default selectors (which contain the platform selector)
	//    3. no selector
	// We work around this by overriding the default selectors with nonsense.
	static const char* envFileSelectors = "QT_FILE_SELECTORS";  // see qfileselector.cpp
	static const char* envOverride = "QT_NO_BUILTIN_SELECTORS"; // see qfileselector.cpp
	qputenv(envFileSelectors, "lalelu");
	qputenv(envOverride, "true");
#endif

	mEngine.reset(new QQmlApplicationEngine());

	mEngine->rootContext()->setContextProperty("plugin", this);
	QQmlFileSelector::get(mEngine.data())->setExtraSelectors(mExplicitPlatformStyle.split(QChar(',')));

	mEngine->rootContext()->setContextProperty("screenDpi", mDpi);
	mEngine->rootContext()->setContextProperty("providerModel", &mProviderModel);
	mEngine->rootContext()->setContextProperty("historyModel", &mHistoryModel);
	mEngine->rootContext()->setContextProperty("changePinModel", &mChangePinModel);
	mEngine->rootContext()->setContextProperty("authModel", &mAuthModel);
	mEngine->rootContext()->setContextProperty("versionInformationModel", &mVersionInformationModel);
	mEngine->rootContext()->setContextProperty("qmlExtension", &mQmlExtension);
	mEngine->rootContext()->setContextProperty("selfAuthenticationModel", &mSelfAuthenticationModel);
	mEngine->rootContext()->setContextProperty("settingsModel", &mSettingsModel);
	mEngine->rootContext()->setContextProperty("certificateDescriptionModel", &mCertificateDescriptionModel);
	mEngine->rootContext()->setContextProperty("chatModel", &mChatModel);
	mEngine->rootContext()->setContextProperty("numberModel", &mNumberModel);
	mEngine->rootContext()->setContextProperty("applicationModel", &mApplicationModel);
	mEngine->rootContext()->setContextProperty("statusBarUtil", &mStatusBarUtil);
	mEngine->rootContext()->setContextProperty("connectivityManager", &mConnectivityManager);

	mEngine->load(getFile(QStringLiteral("qml/main.qml")));

	Updater::getInstance().update();
}


QString UIPlugInQml::getPlatformSelectors() const
{
	static const QString TABLET_SELECTOR_PREFIX("tablet,");

#if defined(Q_OS_ANDROID)
	const jboolean result = QtAndroid::androidActivity().callMethod<jboolean>("isTablet", "()Z");
	const bool isTablet = result != JNI_FALSE;
#else
	// A device with a screen diagnonal above 6 inches is considered a tablet.
	static const double MAX_SMARTPHONE_DIAGONAL_IN = 6.0;
	static const double MAX_SMARTPHONE_DIAGONAL_MM = 25.4 * MAX_SMARTPHONE_DIAGONAL_IN;

	const QList<QScreen*> screens = QGuiApplication::screens();

	Q_ASSERT(!screens.isEmpty());

	QScreen* const mainScreen = screens.first();
	Q_ASSERT(mainScreen != nullptr);

	const QSizeF size = mainScreen->physicalSize();
	const double width = size.width();
	const double height = size.height();
	const double diagonal = sqrt(width * width + height * height);
	const bool isTablet = diagonal > MAX_SMARTPHONE_DIAGONAL_MM;
#endif
	const QString platform = QGuiApplication::platformName();

	return isTablet ? TABLET_SELECTOR_PREFIX + platform : platform;
}


void UIPlugInQml::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mApplicationModel.resetContext(pContext);
	mNumberModel.resetContext(pContext);

	if (auto changePinContext = pContext.objectCast<ChangePinContext>())
	{
		mChangePinModel.resetContext(changePinContext);
	}

	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		mConnectivityManager.startWatching();
		mAuthModel.resetContext(authContext);
		mCertificateDescriptionModel.resetContext(authContext);
		mChatModel.resetContext(authContext);
	}

	if (auto authContext = pContext.objectCast<SelfAuthenticationContext>())
	{
		mSelfAuthenticationModel.resetContext(authContext);
	}
}


void UIPlugInQml::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	mApplicationModel.resetContext();
	mNumberModel.resetContext();

	if (pContext.objectCast<ChangePinContext>())
	{
		mChangePinModel.resetContext();
	}

	if (pContext.objectCast<AuthContext>())
	{
		mConnectivityManager.stopWatching();
		mAuthModel.resetContext();
		mCertificateDescriptionModel.resetContext();
		mChatModel.resetContext();
	}

	if (pContext.objectCast<SelfAuthenticationContext>())
	{
		mSelfAuthenticationModel.resetContext();
	}
}


void UIPlugInQml::doShutdown()
{

}


QUrl UIPlugInQml::getFile(const QString& pFile)
{
	const QString path = FileDestination::getPath(pFile);

#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	if (!QFile::exists(path))
	{
		return QUrl::fromLocalFile(QStringLiteral(RES_DIR) + QLatin1Char('/') + pFile);
	}
#endif

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
	return path;

#else
	return QUrl::fromLocalFile(path);

#endif
}


void UIPlugInQml::doRefresh()
{
	qCDebug(qml) << "Reload qml files";
	QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}


QString UIPlugInQml::getPlatformStyle() const
{
	return mExplicitPlatformStyle;
}


void UIPlugInQml::applyPlatformStyle(const QString& pPlatformStyle)
{
	if (mExplicitPlatformStyle != pPlatformStyle)
	{
		mExplicitPlatformStyle = pPlatformStyle;
		doRefresh();
	}
}


bool UIPlugInQml::isDeveloperBuild() const
{
#ifdef QT_NO_DEBUG
	return false;

#else
	return true;

#endif
}
