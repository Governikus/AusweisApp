/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "LogHandler.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "UIPlugInQml.h"

#include <QFontDatabase>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QStandardPaths>
#include <QtQuickTest>


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

	private:
		const bool mFalse = false;
		const QVariantMap mSafeAreaMargins = {
			{"top", 0}, {"right", 0}, {"bottom", 0}, {"left", 0}
		};
		QString mPlatformStyle;
		QString mFixedFontFamily;

	public:
		QmlTestRunner()
		{
			// Enable this warning again when our minimum Qt version is 5.14
			QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.connections.warning=false"));
		}


		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle)
		{
			Q_UNUSED(pPlatformStyle)
		}

	public Q_SLOTS:
		void applicationAvailable()
		{
			QStandardPaths::setTestModeEnabled(true);
			QCoreApplication::setApplicationName("TestQmlRunner");
			QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));
			ResourceLoader::getInstance().init();
			Env::getSingleton<ReaderManager>()->init();
			UIPlugInQml::registerQmlTypes();
		}


		void cleanupTestCase()
		{
			ResourceLoader::getInstance().shutdown();
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void qmlEngineAvailable(QQmlEngine* pEngine)
		{
			pEngine->rootContext()->setContextProperty(QStringLiteral("plugin"), this);

			connect(pEngine, &QQmlEngine::warnings, [](const QList<QQmlError>& pWarnings){
					bool fail = false;
					for (auto& warning : pWarnings)
					{
						QWARN(warning.toString().toLatin1().constData());
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
						fail |= !warning.description().contains("QML Connections: Implicitly defined onFoo properties in Connections are deprecated. Use this syntax instead:");
#else
						fail = true;
#endif
					}

					if (fail)
					{
						QCoreApplication::quit();
						QFAIL("QQmlEngine has errors");
					}
				});

			const QStringList selectors = QQmlFileSelector::get(pEngine)->selector()->extraSelectors();
			mPlatformStyle = selectors.join(QLatin1String(","));

			mFixedFontFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
		}


};

QUICK_TEST_MAIN_WITH_SETUP(qml, QmlTestRunner)

#include "QmlTestRunner.moc"
