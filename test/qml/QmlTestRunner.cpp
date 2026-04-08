/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "MockNetworkManager.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "UiLoader.h"
#include "UiPluginQml.h"

#include <QDesktopServices>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QtQuickTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_IMPORT_PLUGIN(UiPluginQml)
Q_DECLARE_LOGGING_CATEGORY(init)

class QmlTestRunner
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockNetworkManager> mMockNetworkManager;

	public:
		Q_INVOKABLE void registerUrlScheme(const QString& pScheme)
		{
			QDesktopServices::setUrlHandler(pScheme, this, "urlInvoked");
		}


		Q_INVOKABLE void clearUrlScheme(const QString& pScheme)
		{
			QDesktopServices::unsetUrlHandler(pScheme);
		}

	public Q_SLOTS:
		void applicationAvailable()
		{
			QStandardPaths::setTestModeEnabled(true);
			QCoreApplication::setApplicationName("TestQmlRunner"_L1);
			QCoreApplication::setAttribute(Qt::AA_QtQuickUseDefaultSizePolicy);
			QQuickStyle::setStyle(QStringLiteral("Basic"));
			QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));
			ResourceLoader::getInstance().init();
			Env::getSingleton<ReaderManager>()->init();
			mMockNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mMockNetworkManager.get());
		}


		void cleanupTestCase()
		{
			ResourceLoader::getInstance().shutdown();
			Env::getSingleton<ReaderManager>()->shutdown();
			Env::set(NetworkManager::staticMetaObject, nullptr);
			mMockNetworkManager.reset();
		}


		void qmlEngineAvailable(QQmlEngine* pEngine)
		{
			const auto& prefix = UiPluginQml::adjustQmlImportPath(pEngine);
			pEngine->rootContext()->setContextProperty(QStringLiteral("importPrefix"), prefix);
			pEngine->rootContext()->setContextProperty(QStringLiteral("testRunner"), this);

			connect(pEngine, &QQmlEngine::warnings, this, [](const QList<QQmlError>& pWarnings){
						bool fail = false;
						for (auto& warning : pWarnings)
						{
							qCritical() << warning;
							fail = true;
						}

						if (fail)
						{
							QCoreApplication::quit();
							QFAIL("QQmlEngine has errors");
						}
					});

			Q_UNUSED(Env::getSingleton<UiLoader>()->load())
		}

	Q_SIGNALS:
		void urlInvoked(const QUrl& pUrl);

};

QUICK_TEST_MAIN_WITH_SETUP(qml, QmlTestRunner)

#include "QmlTestRunner.moc"
