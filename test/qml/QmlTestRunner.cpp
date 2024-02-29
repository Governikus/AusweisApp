/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "MockNetworkManager.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "UIPlugInQml.h"

#include <QFontDatabase>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QtQuickTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(init)

class QmlTestRunner
	: public QObject
{
	Q_OBJECT

	// Mock of the UIPluginQml properties
	Q_PROPERTY(bool debugBuild MEMBER mFalse CONSTANT)
	Q_PROPERTY(bool developerVersion MEMBER mFalse CONSTANT)
	Q_PROPERTY(bool dominated MEMBER mFalse CONSTANT)
	Q_PROPERTY(QVariantMap safeAreaMargins MEMBER mSafeAreaMargins CONSTANT)
	Q_PROPERTY(bool highContrastEnabled MEMBER mFalse CONSTANT)
	Q_PROPERTY(QString platformStyle MEMBER mPlatformStyle CONSTANT)
	Q_PROPERTY(QString fixedFontFamily MEMBER mFixedFontFamily CONSTANT)
	Q_PROPERTY(qreal scaleFactor MEMBER mScaleFactor CONSTANT)
	Q_PROPERTY(qreal fontScaleFactor MEMBER mFontScaleFactor CONSTANT)

	private:
		const bool mFalse = false;
		const QVariantMap mSafeAreaMargins = {
			{"top"_L1, 0}, {"right"_L1, 0}, {"bottom"_L1, 0}, {"left"_L1, 0}
		};
		QString mPlatformStyle;
		QString mFixedFontFamily;
		static constexpr qreal mScaleFactor = 0.6;
		static constexpr qreal mFontScaleFactor = 1.0;

		QSharedPointer<MockNetworkManager> mMockNetworkManager;

	public:
		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle)
		{
			Q_UNUSED(pPlatformStyle)
		}

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
			UIPlugInQml::registerQmlTypes();
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
			pEngine->rootContext()->setContextProperty("plugin"_L1, this);
#if (QT_VERSION < QT_VERSION_CHECK(6, 4, 2))
			pEngine->rootContext()->setContextProperty("hasBindingLoop"_L1, true);
#else
			pEngine->rootContext()->setContextProperty("hasBindingLoop"_L1, false);
#endif

			connect(pEngine, &QQmlEngine::warnings, [](const QList<QQmlError>& pWarnings){
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

			const QStringList selectors = QQmlFileSelector(pEngine).selector()->extraSelectors();
			mPlatformStyle = selectors.join(QLatin1String(","));

			mFixedFontFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
		}


};

QUICK_TEST_MAIN_WITH_SETUP(qml, QmlTestRunner)

#include "QmlTestRunner.moc"
