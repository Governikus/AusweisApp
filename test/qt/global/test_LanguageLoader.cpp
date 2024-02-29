/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref LanguageLoader
 */

#include "LanguageLoader.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_LanguageLoader
	: public QObject
{
	Q_OBJECT

	QTemporaryDir mTranslationDir;

	private Q_SLOTS:
		void initTestCase()
		{
			TestFileHelper::createTranslations(mTranslationDir.path());
			LanguageLoader::getInstance().setPath(mTranslationDir.path());
		}


		void cleanup()
		{
			if (LanguageLoader::getInstance().isLoaded())
			{
				LanguageLoader::getInstance().unload();
			}
		}


		void benchmark()
		{
			QBENCHMARK{
				QCOMPARE(LanguageLoader::getInstance().getAvailableLocales().size(), 6);
			}
		}


		void loadTwice()
		{
			QTest::ignoreMessage(QtDebugMsg, "Try to load translation: QLocale(German, Latin, Germany)");
			QVERIFY(!LanguageLoader::getInstance().isLoaded());
			LanguageLoader::getInstance().load(QLocale::German);
			QVERIFY(LanguageLoader::getInstance().isLoaded());

			QTest::ignoreMessage(QtWarningMsg, "Loader is already in use. You need to unload before you load again...");
			LanguageLoader::getInstance().load(QLocale::German);
		}


		void load()
		{
			QTest::ignoreMessage(QtDebugMsg, "Try to load translation: QLocale(French, Latin, France)");
			QLocale locale = QLocale::French;
			LanguageLoader::getInstance().load(locale);
			QVERIFY(LanguageLoader::getInstance().isLoaded());
			QVERIFY(!LanguageLoader::getInstance().mTranslatorList.isEmpty());

			QTest::ignoreMessage(QtWarningMsg, "Loader is already in use. You need to unload before you load again...");
			LanguageLoader::getInstance().load(QLocale::German);
			QVERIFY(LanguageLoader::getInstance().isLoaded());

			QCOMPARE(LanguageLoader::getInstance().getUsedLocale(), locale);
		}


		void unload()
		{
			QVERIFY(!LanguageLoader::getInstance().isLoaded());

			QTest::ignoreMessage(QtWarningMsg, "Cannot unload translation because it is nothing loaded");
			LanguageLoader::getInstance().unload();
			QVERIFY(!LanguageLoader::getInstance().isLoaded());

			QTest::ignoreMessage(QtWarningMsg, "Cannot unload translation because it is nothing loaded");
			LanguageLoader::getInstance().unload();
			QVERIFY(!LanguageLoader::getInstance().isLoaded());

			QTest::ignoreMessage(QtWarningMsg, "Cannot unload translation because it is nothing loaded");
			LanguageLoader::getInstance().unload();
			QVERIFY(!LanguageLoader::getInstance().isLoaded());

			QVERIFY(LanguageLoader::getInstance().mTranslatorList.isEmpty());
		}


		void isLoaded()
		{
			LanguageLoader::mDefaultLanguage = QLocale::German; // avoid QLocale::C of QLocale::system()
			QVERIFY(!LanguageLoader::getInstance().isLoaded());
			LanguageLoader::getInstance().load();
			QVERIFY(LanguageLoader::getInstance().isLoaded());

			LanguageLoader::getInstance().unload();
			QVERIFY(!LanguageLoader::getInstance().isLoaded());
		}


		void localeC()
		{
			QTest::ignoreMessage(QtDebugMsg, R"(Using fallback language: "English")");

			LanguageLoader::getInstance().load(QLocale::C);
			QCOMPARE(LanguageLoader::getInstance().getUsedLocale(), LanguageLoader::getInstance().getFallbackLanguage());

			QVERIFY(LanguageLoader::getInstance().mTranslatorList.isEmpty());
		}


		void usedLocale()
		{
			QTest::ignoreMessage(QtDebugMsg, "Try to load translation: QLocale(German, Latin, Germany)");
			QLocale locale = QLocale::German;
			LanguageLoader::getInstance().load(locale);
			QCOMPARE(LanguageLoader::getInstance().getUsedLocale(), locale);

			QVERIFY(!LanguageLoader::getInstance().mTranslatorList.isEmpty());
		}


		void unknownLocale()
		{
			QTest::ignoreMessage(QtDebugMsg, R"(UI language: "chr")");
			QTest::ignoreMessage(QtDebugMsg, "Try to load translation: QLocale(Cherokee, Cherokee, United States)");

			QTest::ignoreMessage(QtDebugMsg, R"(UI language: "chr-US")");
			QTest::ignoreMessage(QtDebugMsg, "Try to load translation: QLocale(Cherokee, Cherokee, United States)");

			QTest::ignoreMessage(QtDebugMsg, R"(UI language: "chr-Cher-US")");
			QTest::ignoreMessage(QtDebugMsg, "Try to load translation: QLocale(Cherokee, Cherokee, United States)");

			LanguageLoader::getInstance().load(QLocale::Cherokee);
			QVERIFY(!LanguageLoader::getInstance().isLoaded());

			QLocale locale = QLocale::English;
			QCOMPARE(LanguageLoader::getInstance().getUsedLocale(), locale);

			QVERIFY(LanguageLoader::getInstance().mTranslatorList.isEmpty());
		}


		void fallbackLanguage()
		{
			QLocale::Language lang = LanguageLoader::getInstance().getFallbackLanguage();
			QCOMPARE(lang, QLocale::English);

			QTest::ignoreMessage(QtDebugMsg, R"(Using fallback language: "English")");
			LanguageLoader::getInstance().load(QLocale::English);
		}


		void defaultPath()
		{
			LanguageLoader::getInstance().setPath("dummy"_L1);
			QCOMPARE(LanguageLoader::getInstance().getPath(), "dummy"_L1);
		}


};

QTEST_GUILESS_MAIN(test_LanguageLoader)
#include "test_LanguageLoader.moc"
