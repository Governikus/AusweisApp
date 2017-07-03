/*!
 * test_History.h
 *
 * \brief Unit tests for History.
 *
 * All tests ends with _QTEST to be able to identify them later.
 * All original history entry from AusweisApp2 do not have this.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "HistorySettings.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_HistorySettings
	: public QObject
{
	Q_OBJECT
	QScopedPointer<HistorySettings> settings;

	private Q_SLOTS:
		void init()
		{
			QCoreApplication::setOrganizationName(QStringLiteral("dummy"));
			AbstractSettings::mTestDir.clear();
			settings.reset(new HistorySettings());
		}


		void testEquals()
		{
			QScopedPointer<HistorySettings> otherSettings(new HistorySettings());

			QVERIFY(*settings == *otherSettings);

			settings->setEnabled(!settings->isEnabled());
			QVERIFY(*settings != *otherSettings);
			otherSettings->setEnabled(settings->isEnabled());
			QVERIFY(*settings == *otherSettings);

			HistoryEntry entry("pSubjectName", "pSubjectUrl", "pUsage", QDateTime(), "pTermOfUsage", "pRequestedData");
			settings->addHistoryEntry(entry);
			QVERIFY(*settings != *otherSettings);
			otherSettings->addHistoryEntry(entry);
			QVERIFY(*settings == *otherSettings);

		}


		void testEnabled()
		{
			bool initial = settings->isEnabled();

			settings->setEnabled(!initial);
			QCOMPARE(settings->isEnabled(), !initial);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings->setEnabled(initial);
			QCOMPARE(settings->isEnabled(), initial);
			settings->save();
		}


		void testHistoryEntries()
		{
			QVector<HistoryEntry> initial = settings->getHistoryEntries();
			HistoryEntry entry("pSubjectName", "pSubjectUrl", "pUsage", QDateTime(), "pTermOfUsage", "pRequestedData");
			QVector<HistoryEntry> newValue(initial);
			newValue.prepend(entry); // new values will be prepended, so that it appears on top

			settings->addHistoryEntry(entry);
			QCOMPARE(settings->getHistoryEntries(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());
		}


		void testDeleteHistory()
		{
			HistoryEntry entry("pSubjectName", "pSubjectUrl", "pUsage", QDateTime(), "pTermOfUsage", "pRequestedData");
			settings->addHistoryEntry(entry);

			QCOMPARE(settings->getHistoryEntries().size(), 1);

			settings->deleteSettings();

			QCOMPARE(settings->getHistoryEntries().size(), 0);
		}


		void testDeleteHistoryFromFile()
		{
			settings->load();
			const auto file = AbstractSettings::mTestDir->path() + QStringLiteral("/dummy/Test_settings_HistorySettings.ini");

			HistoryEntry entry("pSubjectXYZ", "pSubjectUrlXYZ", "pUsageXYZ", QDateTime(), "pTermOfUsageXYZ", "pRequestedDataXYZ");
			settings->addHistoryEntry(entry);
			settings->addHistoryEntry(entry);
			settings->addHistoryEntry(entry);
			settings->save();
			QVERIFY(QFile::exists(file));

			auto content = TestFileHelper::readFile(file);
			QVERIFY(content.contains("pSubjectXYZ"));
			QVERIFY(content.contains("pSubjectUrlXYZ"));
			QVERIFY(content.contains("pUsageXYZ"));
			QVERIFY(content.contains("pTermOfUsageXYZ"));
			QVERIFY(content.contains("pRequestedDataXYZ"));

			settings->deleteSettings();
			settings->save();

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
