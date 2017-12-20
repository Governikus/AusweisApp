/*!
 * \brief Unit tests for \ref HelpAction
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"

#include <QtTest/QtTest>

#include "generic/HelpAction.h"
#include "LanguageLoader.h"

using namespace governikus;

class test_HelpAction
	: public QObject
{
	Q_OBJECT

	QDir helpDir = QCoreApplication::applicationDirPath() + QStringLiteral("/help/");
	QTemporaryDir mTranslationDir;

	void loadLanguage(QLocale::Language pLang)
	{
		if (LanguageLoader::getInstance().isLoaded())
		{
			LanguageLoader::getInstance().unload();
		}

		LanguageLoader::getInstance().load(pLang);
		QCOMPARE(LanguageLoader::getInstance().getUsedLocale().language(), pLang);
	}


	QString toWebUrl(const QString& pLang = QStringLiteral("en"))
	{
		#ifdef Q_OS_MACOS
		const QString sys = "macOS";
		#else
		const QString sys = "Windows";
		#endif

		return QStringLiteral("https://www.ausweisapp.bund.de/ausweisapp2/handbuch/1.9/%1/%2/index.html").arg(pLang, sys);
	}


	QString toUrl(const char* pStr)
	{
		return QUrl::fromLocalFile(helpDir.path()).toString() + QString::fromLatin1(pStr);
	}


	private Q_SLOTS:
		void init()
		{
			QCoreApplication::setApplicationVersion(QStringLiteral("1.9"));
			QVERIFY(!helpDir.exists());
		}


		void initTestCase()
		{
			TestFileHelper::createTranslations(mTranslationDir.path());
			LanguageLoader::getInstance().setPath(mTranslationDir.path());
		}


		void cleanup()
		{
			QVERIFY(helpDir.removeRecursively());
			if (LanguageLoader::getInstance().isLoaded())
			{
				LanguageLoader::getInstance().unload();
			}
		}


		void escapedMapping()
		{
			QVERIFY(helpDir.mkpath(helpDir.path()));

			QVERIFY(helpDir.mkdir("de"));
			loadLanguage(QLocale::German);

			QString padding;
			if (QSysInfo::prettyProductName().startsWith(QLatin1String("Windows")))
			{
				padding = '/';
			}

			QCOMPARE(HelpAction::getInstance().getHelpUrl("generalTab"), "file://" + padding + helpDir.path() + "/de/settings-general.html");
		}


		void nonExistingHelpDir()
		{
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl());

			loadLanguage(QLocale::English);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl());

			loadLanguage(QLocale::German);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl("de"));

			loadLanguage(QLocale::Italian);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl("it"));
		}


		void existingHelpDir()
		{
			QVERIFY(helpDir.mkpath(helpDir.path()));

			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl());

			loadLanguage(QLocale::English);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl());

			loadLanguage(QLocale::German);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl("de"));

			loadLanguage(QLocale::Italian);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toWebUrl("it"));
		}


		void existingLanguageHelpDir()
		{
			QVERIFY(helpDir.mkpath(helpDir.path()));

			QVERIFY(helpDir.mkdir("en"));
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toUrl("/en/index.html"));

			QVERIFY(helpDir.mkdir("de"));
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toUrl("/en/index.html"));

			loadLanguage(QLocale::German);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toUrl("/de/index.html"));

			loadLanguage(QLocale::Italian);
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toUrl("/en/index.html"));

			LanguageLoader::getInstance().unload();
			QCOMPARE(HelpAction::getInstance().getHelpUrl(""), toUrl("/en/index.html"));
		}


		void contextMap()
		{
			QCOMPARE(HelpAction::getInstance().getContextMapping(""), QString("index.html"));

			QCOMPARE(HelpAction::getInstance().getContextMapping("pinTab"), QString("settings-pin-management.html"));
			QCOMPARE(HelpAction::getInstance().getContextMapping("unknown"), QString("index.html"));
		}


};

QTEST_MAIN(test_HelpAction)
#include "test_HelpAction.moc"
