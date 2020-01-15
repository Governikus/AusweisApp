/*!
 * \brief Unit tests for \ref DiagnosisModel
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisTreeModel.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class test_DiagnosisTreeModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<DiagnosisContext> mContext;
	QSharedPointer<DiagnosisTreeModel> mModel;

	private:
		QString getTestData(const QString& pFilename)
		{
			QString filePath = QStringLiteral(":/core/diagnosis/") + pFilename;
			QByteArray rawData = TestFileHelper::readFile(filePath);
			return QString::fromUtf8(rawData);
		}


	private Q_SLOTS:
		void init()
		{
			mContext.reset(new DiagnosisContext());
			mModel.reset(new DiagnosisTreeModel(mContext));
		}


		void cleanup()
		{
			mModel.clear();
			mContext.clear();
		}


		void test_newDiagnosisTreeModel()
		{
			QCOMPARE(mModel->mContext, mContext);
			QCOMPARE(mModel->mRootItem->getText(), QString("Diagnosis data"));
			QCOMPARE(mModel->mOperatingSystemItem->getText(), QString("Operating system"));
			QCOMPARE(mModel->mReaderItem->getText(), QString("Card reader"));
			QCOMPARE(mModel->mPcScItem->getText(), QString("PC/SC"));
			QCOMPARE(mModel->mPairedDevices->getText(), QString("Paired devices"));
			QCOMPARE(mModel->mNetworkInterfaces->getText(), QString("Network interfaces"));
			QCOMPARE(mModel->mNetworkConnectionTest->getText(), QString("Network connection test"));
			QCOMPARE(mModel->mInstalledAntivirus->getText(), QString("Installed antivirus software"));
			QCOMPARE(mModel->mWindowsFirewall->getText(), QString("Firewall"));
			QCOMPARE(mModel->mTimestampItem->getText(), QString("Time of diagnosis"));

			QCOMPARE(mModel->mRootItem->childCount(), 10);
			QCOMPARE(mModel->mRootItem->getChild(0), mModel->mAppVersionItem);
			QCOMPARE(mModel->mRootItem->getChild(1), mModel->mOperatingSystemItem);
			QCOMPARE(mModel->mRootItem->getChild(2), mModel->mReaderItem);
			QCOMPARE(mModel->mRootItem->getChild(3), mModel->mPcScItem);
			QCOMPARE(mModel->mRootItem->getChild(4), mModel->mPairedDevices);
			QCOMPARE(mModel->mRootItem->getChild(5), mModel->mNetworkInterfaces);
			QCOMPARE(mModel->mRootItem->getChild(6), mModel->mNetworkConnectionTest);
			QCOMPARE(mModel->mRootItem->getChild(7), mModel->mInstalledAntivirus);
			QCOMPARE(mModel->mRootItem->getChild(8), mModel->mWindowsFirewall);
			QCOMPARE(mModel->mRootItem->getChild(9), mModel->mTimestampItem);

			QCOMPARE(mModel->mOperatingSystemItem->childCount(), 3);

			QCOMPARE(mModel->mReaderItem->childCount(), 1);
			QCOMPARE(mModel->mReaderItem->getChild(0)->getText(), QString("Diagnosis is running..."));

			QCOMPARE(mModel->mNetworkConnectionTest->childCount(), 1);
			QCOMPARE(mModel->mNetworkConnectionTest->getChild(0)->getText(), QString("Diagnosis is running..."));
		}


		void test_OnReaderInfosChanged()
		{
			mModel->onReaderInfosChanged();
			QCOMPARE(mModel->mReaderItem->childCount(), 1);
			QCOMPARE(mModel->mReaderItem->getChild(0)->getText(), QString("Not recognised"));

			ReaderInfo defaultInfo;
			ReaderInfo infoEidCard(QString("testInfo"), ReaderManagerPlugInType::PCSC, CardInfo(CardType::EID_CARD));
			ReaderInfo comfortReaderInfo(QString("name"), ReaderManagerPlugInType::BLUETOOTH, CardInfo(CardType::UNKNOWN));
			comfortReaderInfo.setBasicReader(false);
			const QVector<ReaderInfo> readerInfos = {defaultInfo, infoEidCard, comfortReaderInfo};
			mContext->setReaderInfos(readerInfos);
			mModel->onReaderInfosChanged();

			QCOMPARE(mModel->mReaderItem->childCount(), 3);
			QCOMPARE(mModel->mReaderItem->getChild(0)->getText(), QString());
			QCOMPARE(mModel->mReaderItem->getChild(1)->getText(), QString("testInfo"));
			QCOMPARE(mModel->mReaderItem->getChild(2)->getText(), QString("name"));

			auto child1 = mModel->mReaderItem->getChild(0);
			QCOMPARE(child1->childCount(), 2);
			QCOMPARE(child1->getChild(0)->getText(), QString("Type: Basic card reader"));
			QCOMPARE(child1->getChild(1)->getText(), QString("Card: not inserted"));

			auto child2 = mModel->mReaderItem->getChild(1);
			QCOMPARE(child2->childCount(), 3);
			QCOMPARE(child2->getChild(0)->getText(), QString("Type: Basic card reader"));
			QCOMPARE(child2->getChild(1)->getText(), QString("Card: ID card (PA/eAT)"));
			QCOMPARE(child2->getChild(2)->getText(), QString("Retry counter: -1"));

			auto child3 = mModel->mReaderItem->getChild(2);
			QCOMPARE(child3->childCount(), 2);
			QCOMPARE(child3->getChild(0)->getText(), QString("Type: Standard / comfort card reader"));
			QCOMPARE(child3->getChild(1)->getText(), QString("Card: unknown type"));
		}


		void test_onPcscInfoChanged()
		{
			const QString version("version");
			const DiagnosisContext::ComponentInfo component1;
			const DiagnosisContext::ComponentInfo component2;
			const DiagnosisContext::ComponentInfo driver1;
			const DiagnosisContext::ComponentInfo driver2;
			QVector<DiagnosisContext::ComponentInfo> components = {component1, component2};
			QVector<DiagnosisContext::ComponentInfo> drivers = {driver1, driver2};
			mContext->setPcscInfo(version, components, drivers);

			mModel->onPcscInfoChanged();
			QCOMPARE(mModel->mPcScItem->childCount(), 3);
			QCOMPARE(mModel->mPcScItem->getChild(0)->getText(), QString("Version: version"));
			QCOMPARE(mModel->mPcScItem->getChild(1)->getText(), QString("Components"));
			QCOMPARE(mModel->mPcScItem->getChild(2)->getText(), QString("Driver"));
		}


		void test_OnTimestampChanged()
		{
			const QDateTime invalid;
			const QDate date(2018, 10, 12);
			const QTime time(12, 0);
			const QDateTime valid(date, time);

			mContext->setTimestamp(invalid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampItem->childCount(), 1);
			QCOMPARE(mModel->mTimestampItem->getChild(0)->getText(), QString("Initial diagnosis running, please wait."));

			mContext->setTimestamp(valid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampItem->childCount(), 1);
			QCOMPARE(mModel->mTimestampItem->getChild(0)->getText(), QString("12. October 2018, 12:00:00 PM"));
		}


		void test_OnNetworkInfoChanged()
		{
			const QNetworkInterface interface1;
			const QNetworkInterface interface2;
			const QNetworkInterface interface3;
			QList<QNetworkInterface> interfaces = {interface1, interface2, interface3};

			mContext->setNetworkInterfaces(interfaces);
			mModel->onNetworkInfoChanged();
			QCOMPARE(mModel->mNetworkInterfaces->childCount(), 3);
			for (int i = 0; i < mModel->mNetworkInterfaces->childCount(); i++)
			{
				const auto& interface = mModel->mNetworkInterfaces->getChild(i);
				QCOMPARE(interface->childCount(), 2);
			}
		}


		void test_OnAntivirusDetectionFailed()
		{
			mModel->onAntivirusDetectionFailed();
			QCOMPARE(mModel->mInstalledAntivirus->childCount(), 1);
			QCOMPARE(mModel->mInstalledAntivirus->getChild(0)->getText(), QString("Antivirus detection failed."));
		}


		void test_OnFirewallInformationFailed()
		{
			mModel->onFirewallInformationFailed();
			QCOMPARE(mModel->mWindowsFirewall->childCount(), 1);
			QCOMPARE(mModel->mWindowsFirewall->getChild(0)->getText(), QString("An error occurred while trying to gather firewall information. Please check the log for more information."));
		}


		void test_OnConnectionTestDone_data()
		{
			QTest::addColumn<bool>("proxySet");
			QTest::addColumn<bool>("pingTestOnProxy");
			QTest::addColumn<bool>("connectionTestOnProxy");
			QTest::addColumn<bool>("connectionTestWithoutProxy");
			QTest::addColumn<int>("childCount");

			QTest::newRow("proxyConnectionSuccessful") << true << true << true << false << 3;
			QTest::newRow("connectionWithoutProxySuccessful") << false << false << false << true << 2;
			QTest::newRow("proxyConnectionNotSuccessful") << true << false << false << false << 3;
			QTest::newRow("noConnection") << false << false << false << false << 2;
		}


		void test_OnConnectionTestDone()
		{
			QFETCH(bool, proxySet);
			QFETCH(bool, pingTestOnProxy);
			QFETCH(bool, connectionTestOnProxy);
			QFETCH(bool, connectionTestWithoutProxy);
			QFETCH(int, childCount);

			mModel->mConnectionTest.mIsProxySet = proxySet;
			mModel->mConnectionTest.mPingTestOnProxySuccessful = pingTestOnProxy;
			mModel->mConnectionTest.mConnectionTestWithProxySuccessful = connectionTestOnProxy;
			mModel->mConnectionTest.mConnectionTestWithoutProxySuccessful = connectionTestWithoutProxy;

			auto connectionTestModelIndex = mModel->index(6, 0);
			QSignalSpy spyInsert(mModel.data(), &DiagnosisTreeModel::rowsInserted);

			mModel->onConnectionTestDone();

			if (proxySet)
			{
				QCOMPARE(mModel->mNetworkConnectionTest->childCount(), childCount);

				auto proxy = mModel->mNetworkConnectionTest->getChild(0);

				QCOMPARE(proxy->getText(), QString("Proxy"));
				QCOMPARE(proxy->childCount(), 5);
				QVERIFY(proxy->getChild(0)->getText().contains("Hostname:"));
				QVERIFY(proxy->getChild(1)->getText().contains("Port:"));
				QVERIFY(proxy->getChild(2)->getText().contains("Type:"));
				QVERIFY(proxy->getChild(3)->getText().contains("Capabilities:"));
				QCOMPARE(proxy->getChild(4)->getText(), QString("Ping test to proxy: %1").arg(pingTestOnProxy ? QString("Successful") : QString("Failed")));
				QCOMPARE(mModel->mNetworkConnectionTest->getChild(1)->getText(), QString("Connection test with proxy: %1").arg(connectionTestOnProxy ? QString("Successful") : QString("Failed")));
				QCOMPARE(spyInsert.count(), 1);
				QList<QVariant> argumentsInsert = spyInsert.takeFirst();
				QCOMPARE(argumentsInsert.at(0), connectionTestModelIndex);
				QCOMPARE(argumentsInsert.at(1).toInt(), 0);
				QCOMPARE(argumentsInsert.at(2).toInt(), 2);
			}
			else
			{
				QCOMPARE(mModel->mNetworkConnectionTest->childCount(), childCount);
				QCOMPARE(mModel->mNetworkConnectionTest->getChild(0)->getText(), QString("No Proxy Found"));
				QCOMPARE(spyInsert.count(), 1);
				QList<QVariant> argumentsInsert = spyInsert.takeFirst();
				QCOMPARE(argumentsInsert.at(0), connectionTestModelIndex);
				QCOMPARE(argumentsInsert.at(1).toInt(), 0);
				QCOMPARE(argumentsInsert.at(2).toInt(), 1);
			}
			QCOMPARE(mModel->mNetworkConnectionTest->getChild(childCount - 1)->getText(), QString("Connection test without proxy: %1").arg(connectionTestWithoutProxy ? QString("Successful") : QString("Failed")));
		}


		void test_OnAntivirusInformationChanged()
		{
			auto installedAntivirusModelIndex = mModel->index(7, 0);
			QSignalSpy spyInsert(mModel.data(), &DiagnosisTreeModel::rowsInserted);
			mModel->onAntivirusInformationChanged();
			QCOMPARE(mModel->mInstalledAntivirus->childCount(), 1);
			QCOMPARE(mModel->mInstalledAntivirus->getChild(0)->getText(), QString("No Antivirus software detected."));
			QCOMPARE(spyInsert.count(), 1);
			QList<QVariant> argumentsInsert1 = spyInsert.takeFirst();
			QCOMPARE(argumentsInsert1.at(0), installedAntivirusModelIndex);
			QCOMPARE(argumentsInsert1.at(1).toInt(), 0);
			QCOMPARE(argumentsInsert1.at(2).toInt(), 0);

			const QString& fileContent = getTestData(QStringLiteral("antivir_two_antivirus.txt"));
			mModel->mAntivirusDetection.parseAntivirInfos(fileContent);
			mModel->onAntivirusInformationChanged();
			QCOMPARE(mModel->mInstalledAntivirus->childCount(), 2);

			auto antivirus1 = mModel->mInstalledAntivirus->getChild(0);
			QCOMPARE(antivirus1->getText(), QString("BullGuard Antivirus"));
			QCOMPARE(antivirus1->getChild(0)->getText(), QString("Last updated: Fri, 30 Nov 2018 15:04:13 GMT"));
			QCOMPARE(antivirus1->getChild(1)->getText(), QString("Executable path: C:\\Program Files\\BullGuard Ltd\\BullGuard\\BullGuard.exe"));

			auto antivirus2 = mModel->mInstalledAntivirus->getChild(1);
			QCOMPARE(antivirus2->getText(), QString("Windows Defender"));
			QCOMPARE(antivirus2->getChild(0)->getText(), QString("Last updated: Mon, 26 Nov 2018 10:34:23 GMT"));
			QCOMPARE(antivirus2->getChild(1)->getText(), QString("Executable path: windowsdefender://"));

			QList<QVariant> argumentsInsert2 = spyInsert.takeFirst();
			QCOMPARE(argumentsInsert2.at(0), installedAntivirusModelIndex);
			QCOMPARE(argumentsInsert2.at(1).toInt(), 0);
			QCOMPARE(argumentsInsert2.at(2).toInt(), 1);
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisTreeModel)
#include "test_DiagnosisTreeModel.moc"
