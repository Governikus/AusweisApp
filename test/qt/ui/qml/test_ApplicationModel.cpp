/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ApplicationModel
 */

#include "ApplicationModel.h"

#include <QtTest>

using namespace governikus;

class test_ApplicationModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_stripHtmlTags_data()
		{
			QTest::addColumn<QString>("rawText");
			QTest::addColumn<QString>("result");

			QTest::addRow("empty text") << QString() << QString();
			QTest::addRow("without tags") << QStringLiteral("Hello World!") << QStringLiteral("Hello World!");
			QTest::addRow("with tags") << QStringLiteral("<p>Hello World!</p>") << QStringLiteral("Hello World!");
			QTest::addRow("with <br> tags") << QStringLiteral("<p>Hello<br>World!</p>") << QStringLiteral("Hello World!");
			QTest::addRow("with multiple <br> tags") << QStringLiteral("<p>Hello<br>World!<br><br>How are you?</p>") << QStringLiteral("Hello World! How are you?");
			QTest::addRow("variaty of tags") << QStringLiteral("<h2><a href=\"https://www.example.org\">Hello</a> <marquee>World</marquee></h2><b>!</b>") << QStringLiteral("Hello World!");
		}


		void test_stripHtmlTags()
		{
			QFETCH(QString, rawText);
			QFETCH(QString, result);

			const auto model = Env::getSingleton<ApplicationModel>();
			QCOMPARE(model->stripHtmlTags(rawText), result);
		}


};

QTEST_MAIN(test_ApplicationModel)
#include "test_ApplicationModel.moc"
