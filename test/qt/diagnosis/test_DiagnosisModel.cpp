/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref DiagnosisModel
 */

#include "DiagnosisModel.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class test_DiagnosisModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<DiagnosisContext> mContext;
	QSharedPointer<DiagnosisModel> mModel;

	private:
		QString getTestData(const QString& pFilename)
		{
			QString filePath = QStringLiteral(":/core/diagnosis/") + pFilename;
			QByteArray rawData = TestFileHelper::readFile(filePath);
			return QString::fromUtf8(rawData);
		}


		bool verifyOrder(const QSharedPointer<SectionModel>& pSection,
			const QVector<QVector<ContentItem>>& pOrder)
		{
			const auto& content = pSection->mContentItems;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			qsizetype offset = 0;
#else
			int offset = 0;
#endif
			for (const auto& subsection : pOrder)
			{
				const auto& slice = content.mid(offset, subsection.size());
				if (slice != subsection)
				{
					return false;
				}
				offset += subsection.size();
			}

			return offset == content.size();
		}

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new DiagnosisContext());
			mModel.reset(new DiagnosisModel(mContext));
			mModel->reloadContent();
		}


		void cleanup()
		{
			mModel.clear();
			mContext.clear();
		}


		void test_newDiagnosisModel()
		{
			QCOMPARE(mModel->mContext, mContext);
			QCOMPARE(mModel->rowCount(), mModel->mSections.size());

			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::GENERAL), QCoreApplication::applicationName());
			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::READER), QString("Card reader"));
			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::NETWORK), QString("Network"));
			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::SECURITY), QString("Antivirus and firewall"));

			QCOMPARE(mModel->mSections[DiagnosisModel::Section::GENERAL]->mContentItems.last().mTitle, QString("Time of diagnosis"));
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::GENERAL]->mContentItems.last().mContent, QString("Initial diagnosis running, please wait."));

			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(0).mTitle, QString("Paired smartphones"));
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(1).mContent, QString("No devices paired."));
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(2).mTitle, QString("Card reader"));
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(3).mContent, QString("Diagnosis is running..."));
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(4).mTitle, QString("PC/SC driver information"));
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(5).mContent, QString("Diagnosis is running..."));

			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::READER],
					{mModel->mRemoteDeviceSection, mModel->mCardReaderSection, mModel->mPcscSection}));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::NETWORK],
					{mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY],
					{mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnReaderInfosChanged()
		{
			mModel->onReaderInfosChanged();
			QCOMPARE(mModel->mCardReaderSection.size(), 2);
			QCOMPARE(mModel->mCardReaderSection.at(1).mContent, QString("No supported reader found."));

			ReaderInfo defaultInfo;
			ReaderInfo infoEidCard(QString("testInfo"), ReaderManagerPlugInType::PCSC, CardInfo(CardType::EID_CARD));
			ReaderInfo comfortReaderInfo(QString("name"), ReaderManagerPlugInType::REMOTE_IFD, CardInfo(CardType::UNKNOWN));
			comfortReaderInfo.setBasicReader(false);
			const QVector<ReaderInfo> readerInfos = {defaultInfo, infoEidCard, comfortReaderInfo};
			mContext->setReaderInfos(readerInfos);
			mModel->onReaderInfosChanged();

			QCOMPARE(mModel->mCardReaderSection.size(), 4);

			QCOMPARE(mModel->mCardReaderSection.at(0).mTitle, QString("Connected Card readers"));
			QCOMPARE(mModel->mCardReaderSection.at(1).mTitle, QString());
			QCOMPARE(mModel->mCardReaderSection.at(2).mTitle, QString("testInfo"));
			QCOMPARE(mModel->mCardReaderSection.at(3).mTitle, QString("name"));

			QCOMPARE(mModel->mCardReaderSection.at(0).mContent, QString());
			QCOMPARE(mModel->mCardReaderSection.at(1).mContent, QString("Type: Basic card reader\nCard: not inserted"));
			QCOMPARE(mModel->mCardReaderSection.at(2).mContent, QString("Type: Basic card reader\nCard: ID card (PA/eAT/eID)\nRetry counter: -1"));
			QCOMPARE(mModel->mCardReaderSection.at(3).mContent, QString("Type: Standard / comfort card reader\nCard: unknown type"));

			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::READER], {
						mModel->mRemoteDeviceSection,
						mModel->mCardReaderSection,
						mModel->mPcscSection}));
		}


		void test_OnRemoteInfosChanged()
		{
			mModel->onRemoteInfosChanged();
			QCOMPARE(mModel->mRemoteDeviceSection.size(), 2);
			QCOMPARE(mModel->mRemoteDeviceSection.at(0).mTitle, QString("Paired smartphones"));
			QCOMPARE(mModel->mRemoteDeviceSection.at(1).mContent, QString("No devices paired."));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::READER], {
						mModel->mRemoteDeviceSection,
						mModel->mCardReaderSection,
						mModel->mPcscSection}));
		}


		void test_OnPcscInfoChanged()
		{
			const QString version("Version");
			const DiagnosisContext::ComponentInfo component1(QString("/path/to/component1"), QString("description1"), QString("version1"), QString("vendor1"));
			const DiagnosisContext::ComponentInfo component2(QString("/path/to/component2"), QString("description2"), QString("version2"), QString("vendor2"));
			const DiagnosisContext::ComponentInfo driver1(QString("/path/to/driver1"), QString("description1"), QString("version1"), QString("vendor1"));
			const DiagnosisContext::ComponentInfo driver2(QString("/path/to/driver2"), QString("description2"), QString("version2"), QString("vendor2"));
			QVector<DiagnosisContext::ComponentInfo> components = {component1, component2};
			QVector<DiagnosisContext::ComponentInfo> drivers = {driver1, driver2};
			mContext->setPcscInfo(version, components, drivers);
			mModel->onPcscInfoChanged();

			QCOMPARE(mModel->mPcscSection.size(), 4);

			QCOMPARE(mModel->mPcscSection.at(0).mTitle, QString("PC/SC information"));
			QCOMPARE(mModel->mPcscSection.at(1).mContent, QString("Version"));
			QCOMPARE(mModel->mPcscSection.at(2).mTitle, QString("Components"));
			QCOMPARE(mModel->mPcscSection.at(2).mContent, QString("description1\nVendor: vendor1\nVersion: version1\nFile path: /path/to/component1\ndescription2\nVendor: vendor2\nVersion: version2\nFile path: /path/to/component2"));
			QCOMPARE(mModel->mPcscSection.at(3).mTitle, QString("Driver"));
			QCOMPARE(mModel->mPcscSection.at(3).mContent, QString("description1\nVendor: vendor1\nVersion: version1\nFile path: /path/to/driver1\ndescription2\nVendor: vendor2\nVersion: version2\nFile path: /path/to/driver2"));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::READER], {
						mModel->mRemoteDeviceSection,
						mModel->mCardReaderSection,
						mModel->mPcscSection}));
		}


		void test_OnTimestampChanged()
		{
			const QDateTime invalid;
			const QDate date(2018, 10, 12);
			const QTime time(12, 0);
			const QDateTime valid(date, time);

			mContext->setTimestamp(invalid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampSection.last().mTitle, QString("Time of diagnosis"));
			QCOMPARE(mModel->mTimestampSection.last().mContent, QString("Failed to retrieve date & time"));

			mContext->setTimestamp(valid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampSection.last().mTitle, QString("Time of diagnosis"));
			QCOMPARE(mModel->mTimestampSection.last().mContent, QString("12. October 2018, 12:00:00 PM"));
		}


		void test_OnNetworkInfoChanged()
		{
			const QNetworkInterface interface1;
			const QNetworkInterface interface2;
			const QNetworkInterface interface3;
			QList<QNetworkInterface> interfaces = {interface1, interface2, interface3};
			mContext->setNetworkInterfaces(interfaces);
			mModel->onNetworkInfoChanged();

			QCOMPARE(mModel->mNetworkInterfaceSection.size(), 3);
			for (const auto& item : std::as_const(mModel->mNetworkInterfaceSection))
			{
				QCOMPARE(item.mTitle, QString(""));
				QCOMPARE(item.mContent, QString("Hardware address: <Not set>\nNo IP addresses assigned"));
			}

			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::NETWORK], {mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
		}


		void test_OnAntivirusDetectionFailed()
		{
			mModel->onAntivirusDetectionFailed();
			QCOMPARE(mModel->mAntivirusSection.size(), 1);
			QCOMPARE(mModel->mAntivirusSection.at(0).mTitle, QString("Antivirus information"));
			QCOMPARE(mModel->mAntivirusSection.at(0).mContent, QString("Antivirus detection failed."));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnAntivirusInformationChanged()
		{
			mModel->onAntivirusInformationChanged();
			QCOMPARE(mModel->mAntivirusSection.size(), 1);
			QCOMPARE(mModel->mAntivirusSection.at(0).mTitle, QString("Antivirus information"));
			QCOMPARE(mModel->mAntivirusSection.at(0).mContent, QString("No Antivirus software detected."));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));

			const QString& fileContent = getTestData(QStringLiteral("antivir_two_antivirus.txt"));
			mModel->mAntivirusDetection.parseAntivirInfos(fileContent);
			mModel->onAntivirusInformationChanged();

			QCOMPARE(mModel->mAntivirusSection.size(), 3);
			QCOMPARE(mModel->mAntivirusSection.at(0).mTitle, QString("Antivirus information"));
			QCOMPARE(mModel->mAntivirusSection.at(1).mTitle, QString("BullGuard Antivirus"));
			QCOMPARE(mModel->mAntivirusSection.at(1).mContent, QString("Last updated: Fri, 30 Nov 2018 15:04:13 GMT\nExecutable path: C:\\Program Files\\BullGuard Ltd\\BullGuard\\BullGuard.exe"));
			QCOMPARE(mModel->mAntivirusSection.at(2).mTitle, QString("Windows Defender"));
			QCOMPARE(mModel->mAntivirusSection.at(2).mContent, QString("Last updated: Mon, 26 Nov 2018 10:34:23 GMT\nExecutable path: windowsdefender://"));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnFirewallInformationFailed()
		{
			mModel->onFirewallInformationFailed();
			QCOMPARE(mModel->mFirewallSection.size(), 1);
			QCOMPARE(mModel->mFirewallSection.at(0).mTitle, QString("Firewall information"));
			QCOMPARE(mModel->mFirewallSection.at(0).mContent, QString("An error occurred while trying to gather firewall information. Please check the log for more information."));
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnConnectionTestDone_data()
		{
			QTest::addColumn<bool>("proxySet");
			QTest::addColumn<bool>("pingTestOnProxy");
			QTest::addColumn<bool>("connectionTestOnProxy");
			QTest::addColumn<bool>("connectionTestWithoutProxy");

			QTest::newRow("proxyConnectionSuccessful") << true << true << true << false;
			QTest::newRow("connectionWithoutProxySuccessful") << false << false << false << true;
			QTest::newRow("proxyConnectionNotSuccessful") << true << false << false << false;
			QTest::newRow("noConnection") << false << false << false << false;
		}


		void test_OnConnectionTestDone()
		{
			QFETCH(bool, proxySet);
			QFETCH(bool, pingTestOnProxy);
			QFETCH(bool, connectionTestOnProxy);
			QFETCH(bool, connectionTestWithoutProxy);

			mModel->mConnectionTest.mIsProxySet = proxySet;
			mModel->mConnectionTest.mPingTestOnProxySuccessful = pingTestOnProxy;
			mModel->mConnectionTest.mConnectionTestWithProxySuccessful = connectionTestOnProxy;
			mModel->mConnectionTest.mConnectionTestWithoutProxySuccessful = connectionTestWithoutProxy;

			mModel->onConnectionTestDone();
			QCOMPARE(mModel->mNetworkConnectionSection.size(), 2);
			QCOMPARE(mModel->mNetworkConnectionSection.at(0).mTitle, QString("Service addresses"));
			QCOMPARE(mModel->mNetworkConnectionSection.at(1).mTitle, QString("Proxy information"));

			QString contentString;
			if (proxySet)
			{
				contentString = QString("Hostname: \nPort: \nType: \nCapabilities: \nPing test to proxy: ");
				contentString += pingTestOnProxy ? QString("Successful") : QString("Failed");
				contentString += QString("\nConnection test with proxy: ");
				contentString += connectionTestOnProxy ? QString("Successful") : QString("Failed");
			}
			else
			{
				contentString = QString("No proxy found");
			}
			contentString += QString("\nConnection test without proxy: ");
			contentString += connectionTestWithoutProxy ? QString("Successful") : QString("Failed");

			QCOMPARE(mModel->mNetworkConnectionSection.at(0).mContent, QStringLiteral("Not bound"));
			QCOMPARE(mModel->mNetworkConnectionSection.at(1).mContent, contentString);
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::NETWORK], {mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisModel)
#include "test_DiagnosisModel.moc"
