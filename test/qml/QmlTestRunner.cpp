/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "MockNetworkManager.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "UiLoader.h"
#include "UiPluginQml.h"

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

	public Q_SLOTS:
		void applicationAvailable()
		{
			QStandardPaths::setTestModeEnabled(true);
			QCoreApplication::setApplicationName("TestQmlRunner"_L1);
			QQuickStyle::setStyle(QStringLiteral("Basic"));
			QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));
			ResourceLoader::getInstance().init();
			Env::getSingleton<ReaderManager>()->init();
			mMockNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mMockNetworkManager.get());
			UiPluginQml::registerQmlTypes();
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
#if (QT_VERSION < QT_VERSION_CHECK(6, 4, 2))
			pEngine->rootContext()->setContextProperty(QStringLiteral("hasBindingLoop"), true);
#else
			pEngine->rootContext()->setContextProperty(QStringLiteral("hasBindingLoop"), false);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 2))
			pEngine->rootContext()->setContextProperty(QStringLiteral("hasPolishLoop"), true);
#else
			pEngine->rootContext()->setContextProperty(QStringLiteral("hasPolishLoop"), false);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(6, 6, 0))
			pEngine->rootContext()->setContextProperty(QStringLiteral("canUseTypeCast"), false);
#else
			// Even though some UTs, that use casts, would fail on 6.4 and 6.5, the app itself is not
			// affected and works without any issues or concerning log outputs.
			pEngine->rootContext()->setContextProperty(QStringLiteral("canUseTypeCast"), true);
#endif

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


};

QUICK_TEST_MAIN_WITH_SETUP(qml, QmlTestRunner)

#include "QmlTestRunner.moc"
