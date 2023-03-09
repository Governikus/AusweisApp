/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for History.
 *
 * All tests end with _QTEST to be able to identify them later.
 * All original history entry from AusweisApp2 do not have this.
 */

#include "HistorySettings.h"

#include "AppSettings.h"
#include "Env.h"
#include "VolatileSettings.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_HistorySettings
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			QCoreApplication::setOrganizationName(QStringLiteral("dummy"));
			Env::getSingleton<AppSettings>()->getHistorySettings().deleteSettings();
		}


		void testEnabled()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
			bool initial = settings.isEnabled();

			settings.setEnabled(!initial);
			QCOMPARE(settings.isEnabled(), !initial);

			settings.setEnabled(initial);
			QCOMPARE(settings.isEnabled(), initial);
		}


		void testHistoryEntries()
		{
			SDK_MODE(false);
			auto& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
			QVector<HistoryInfo> initial = settings.getHistoryInfos();
			HistoryInfo info("pSubjectName", "pSubjectUrl", "pUsage", QDateTime(), "pTermOfUsage", {"pRequestedData"});
			QVector<HistoryInfo> newValue(initial);
			newValue.prepend(info); // new values will be prepended, so that it appears on top

			settings.addHistoryInfo(info);
			QCOMPARE(settings.getHistoryInfos(), newValue);
		}


		void testDeleteHistory()
		{
			SDK_MODE(false);
			auto& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
			HistoryInfo info("pSubjectName", "pSubjectUrl", "pUsage", QDateTime(), "pTermOfUsage", {"pRequestedData"});
			settings.addHistoryInfo(info);

			QCOMPARE(settings.getHistoryInfos().size(), 1);

			settings.deleteSettings();

			QCOMPARE(settings.getHistoryInfos().size(), 0);
		}


		void testDeleteHistoryFromFile()
		{
			SDK_MODE(false);
			auto& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
			const auto file = AbstractSettings::mTestDir->path() + QStringLiteral("/dummy/Test_settings_HistorySettings.ini");

			HistoryInfo info("pSubjectXYZ", "pSubjectUrlXYZ", "pUsageXYZ", QDateTime(), "pTermOfUsageXYZ", {"pRequestedDataXYZ"});
			settings.addHistoryInfo(info);
			settings.addHistoryInfo(info);
			settings.addHistoryInfo(info);
			QVERIFY(QFile::exists(file));

			auto content = TestFileHelper::readFile(file);
			QVERIFY(content.contains("pSubjectXYZ"));
			QVERIFY(content.contains("pSubjectUrlXYZ"));
			QVERIFY(content.contains("pUsageXYZ"));
			QVERIFY(content.contains("pTermOfUsageXYZ"));
			QVERIFY(content.contains("pRequestedDataXYZ"));

			settings.deleteSettings();

			content = TestFileHelper::readFile(file);
			QVERIFY(!content.contains("pSubjectXYZ"));
			QVERIFY(!content.contains("pSubjectUrlXYZ"));
			QVERIFY(!content.contains("pUsageXYZ"));
			QVERIFY(!content.contains("pTermOfUsageXYZ"));
			QVERIFY(!content.contains("pRequestedDataXYZ"));
		}


};

QTEST_GUILESS_MAIN(test_HistorySettings)
#include "test_HistorySettings.moc"
