/*!
 * \brief Unit tests for \ref DiagnosisAntivirusDetection
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisFirewallDetection.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class test_DiagnosisFirewallDetection
	: public QObject
{
	Q_OBJECT
	QSharedPointer<DiagnosisFirewallDetection> mFirewallDetection;

	private:
		const QString mTestFileLocation = QStringLiteral(":/core/diagnosis/");
		QString getTestData(const QString& pFilename)
		{
			QString filePath = mTestFileLocation + pFilename;
			QByteArray rawData = TestFileHelper::readFile(filePath);
			return QString::fromUtf8(rawData);
		}

	private Q_SLOTS:
		void init()
		{
			mFirewallDetection.reset(new DiagnosisFirewallDetection());
		}


		void cleanup()
		{
			mFirewallDetection.clear();
		}


		void defaultValues()
		{
			QVERIFY(!mFirewallDetection->getFirstRuleExists());
			QVERIFY(!mFirewallDetection->getFirstRuleEnabled());
			QVERIFY(!mFirewallDetection->getSecondRuleExists());
			QVERIFY(!mFirewallDetection->getSecondRuleEnabled());
			QVERIFY(mFirewallDetection->getFirewallProfiles().empty());
			QVERIFY(mFirewallDetection->getDetectedFirewalls().empty());
		}


#if defined(Q_OS_WIN)


		void test_parsingFirstRuleParameter_data()
		{
			QTest::addColumn<QString>("filename");
			QTest::addColumn<bool>("enabled");
			QTest::addColumn<bool>("exists");

			QTest::newRow("enabled") << QStringLiteral("first_Rule.txt") << true << true;
			QTest::newRow("disabled") << QStringLiteral("firstRuleDisabled.txt") << false << true;
			QTest::newRow("missing") << QStringLiteral("emptyResponse.txt") << false << false;
		}


		void test_parsingFirstRuleParameter()
		{
			QFETCH(QString, filename);
			QFETCH(bool, enabled);
			QFETCH(bool, exists);

			QSignalSpy spy(mFirewallDetection.data(), &DiagnosisFirewallDetection::fireFirewallInformationReady);
			QVERIFY(!mFirewallDetection->getFirstRuleExists());
			QVERIFY(!mFirewallDetection->getFirstRuleEnabled());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(filename);
			mFirewallDetection->parseFirewallFirstRuleInfos(fileContent);
			auto firstRuleExistsInfos = mFirewallDetection->getFirstRuleExists();
			auto firstRuleEnabledInfos = mFirewallDetection->getFirstRuleEnabled();
			QCOMPARE(firstRuleExistsInfos, exists);
			QCOMPARE(firstRuleEnabledInfos, enabled);
			QCOMPARE(spy.count(), 1);
		}


		void test_parsingSecondRuleParameter_data()
		{
			QTest::addColumn<QString>("filename");
			QTest::addColumn<bool>("enabled");
			QTest::addColumn<bool>("exists");

			QTest::newRow("enabled") << QStringLiteral("second_Rule.txt") << true << true;
			QTest::newRow("disabled") << QStringLiteral("secondRuleDisabled.txt") << false << true;
			QTest::newRow("missing") << QStringLiteral("emptyResponse.txt") << false << false;
		}


		void test_parsingSecondRuleParameter()
		{
			QFETCH(QString, filename);
			QFETCH(bool, enabled);
			QFETCH(bool, exists);
			QSignalSpy spy(mFirewallDetection.data(), &DiagnosisFirewallDetection::fireFirewallInformationReady);
			QVERIFY(!mFirewallDetection->getSecondRuleExists());
			QVERIFY(!mFirewallDetection->getSecondRuleEnabled());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(filename);
			mFirewallDetection->parseFirewallSecondRuleInfos(fileContent);
			auto secondRuleExistsInfos = mFirewallDetection->getSecondRuleExists();
			auto secondRuleEnabledInfos = mFirewallDetection->getSecondRuleEnabled();
			QCOMPARE(secondRuleExistsInfos, exists);
			QCOMPARE(secondRuleEnabledInfos, enabled);
			QCOMPARE(spy.count(), 1);
		}


		void test_parsingProfileParameter_data()
		{
			QTest::addColumn<QString>("filename");
			QTest::addColumn<bool>("enabledDomain");
			QTest::addColumn<bool>("enabledPrivate");
			QTest::addColumn<bool>("enabledPublic");
			QTest::addColumn<int>("size");

			QTest::newRow("enabled") << QStringLiteral("firewall_profiles.txt") << true << true << true << 3;
			QTest::newRow("disabled") << QStringLiteral("firewall_profiles_disabled.txt") << false << false << false << 3;
			QTest::newRow("domain_disabled") << QStringLiteral("firewall_profiles_domain_disabled.txt") << false << true << true << 3;
			QTest::newRow("domain_private_disabled") << QStringLiteral("firewall_profiles_domain_private_disabled.txt") << false << false << true << 3;
			QTest::newRow("domain_public_disbaled") << QStringLiteral("firewall_profiles_domain_public_disabled.txt") << false << true << false << 3;
			QTest::newRow("private_disabled") << QStringLiteral("firewall_profiles_private_disabled.txt") << true << false << true << 3;
			QTest::newRow("private_public_disabled") << QStringLiteral("firewall_profiles_private_public_disabled.txt") << true << false << false << 3;
			QTest::newRow("public_disabled") << QStringLiteral("firewall_profiles_public_disabled.txt") << true << true << false << 3;
			QTest::newRow("missing") << QStringLiteral("emptyResponse.txt") << false << false << false << 0;
		}


		void test_parsingProfileParameter()
		{
			QFETCH(QString, filename);
			QFETCH(bool, enabledDomain);
			QFETCH(bool, enabledPrivate);
			QFETCH(bool, enabledPublic);
			QFETCH(int, size);

			QSignalSpy spy(mFirewallDetection.data(), &DiagnosisFirewallDetection::fireFirewallInformationReady);
			QVERIFY(mFirewallDetection->getFirewallProfiles().empty());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(filename);
			mFirewallDetection->parseFirewallProfileInfos(fileContent);
			auto firewallProfileInfos = mFirewallDetection->getFirewallProfiles();
			QCOMPARE(firewallProfileInfos.size(), size);
			if (size > 0)
			{
				QCOMPARE(firewallProfileInfos[0]->getName(), "Domain");
				QCOMPARE(firewallProfileInfos[0]->getEnabled(), enabledDomain);
				QCOMPARE(firewallProfileInfos[1]->getName(), "Private");
				QCOMPARE(firewallProfileInfos[1]->getEnabled(), enabledPrivate);
				QCOMPARE(firewallProfileInfos[2]->getName(), "Public");
				QCOMPARE(firewallProfileInfos[2]->getEnabled(), enabledPublic);
			}

			QCOMPARE(spy.count(), 1);
		}


		void test_parsingFirewallSoftwareParameter_data()
		{
			QTest::addColumn<QString>("filename");
			QTest::addColumn<QString>("name");

			QTest::newRow("kaspersky") << QStringLiteral("firewallSoftwareParametersKaspersky.txt") << QString("Kaspersky Internet Security");
			QTest::newRow("bullguard") << QStringLiteral("firewallSoftwareParametersBullguard.txt") << QString("BullGuard Firewall");
			QTest::newRow("missing") << QStringLiteral("emptyResponse.txt") << QString("");
		}


		void test_parsingFirewallSoftwareParameter()
		{
			QFETCH(QString, filename);
			QFETCH(QString, name);

			QSignalSpy spy(mFirewallDetection.data(), &DiagnosisFirewallDetection::fireFirewallInformationReady);
			QVERIFY(mFirewallDetection->getDetectedFirewalls().empty());
			QCOMPARE(spy.count(), 0);

			const QString& fileContent = getTestData(filename);
			mFirewallDetection->parseFirewallSoftwareInfos(fileContent);
			auto firewallSoftwareInfos = mFirewallDetection->getDetectedFirewalls();

			if (!name.isEmpty())
			{
				QCOMPARE(firewallSoftwareInfos.size(), 1);
				QCOMPARE(firewallSoftwareInfos[0]->getName(), name);
				QCOMPARE(firewallSoftwareInfos[0]->getEnabled(), true);
				QCOMPARE(firewallSoftwareInfos[0]->getUpToDate(), true);
			}
			else
			{
				QVERIFY(firewallSoftwareInfos.empty());
			}

			QCOMPARE(spy.count(), 1);
		}


#endif

};

QTEST_GUILESS_MAIN(test_DiagnosisFirewallDetection)
#include "test_DiagnosisFirewallDetection.moc"
