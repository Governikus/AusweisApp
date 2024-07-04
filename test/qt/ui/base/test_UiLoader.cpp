/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiLoader.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_UiLoader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void environment()
		{
			const QByteArray jsonString(R"(
				{"IID":"governikus.UIPlugin",
					"MetaData": {
						"default":true,
						"env":{"TEST_UILOADER":"dummy"},
						"userInteractive":true
					},
					"className":"UIPluginQml"}
			)");

			QJsonParseError jsonError {};
			const auto& json = QJsonDocument::fromJson(jsonString, &jsonError);
			QCOMPARE(jsonError.error, QJsonParseError::NoError);
			const auto& obj = json.object();

			const char* envVarName = "TEST_UILOADER";
			qunsetenv(envVarName);

			QTest::ignoreMessage(QtDebugMsg, "Set environment: \"TEST_UILOADER\" = \"dummy\"");
			UiLoader::setEnvironment(obj);
			QCOMPARE(qgetenv(envVarName), "dummy");
		}


};

QTEST_GUILESS_MAIN(test_UiLoader)
#include "test_UiLoader.moc"
