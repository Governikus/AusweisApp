/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref DiagnosisModel
 */

#include "DiagnosisModel.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_DiagnosisModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<DiagnosisModel> mModel;

	private:
		QString getTestData(const QString& pFilename)
		{
			const QString filePath = QStringLiteral(":/core/diagnosis/") + pFilename;
			const QByteArray rawData = TestFileHelper::readFile(filePath);
			QString stringData = QString::fromUtf8(rawData);

			Q_ASSERT(stringData.count("\r\n"_L1) == 0);
			const qsizetype linebreaks = stringData.count('\n'_L1);
			stringData.replace('\n'_L1, "\r\n"_L1);
			Q_ASSERT(stringData.count("\r\n"_L1) == linebreaks);

			return stringData;
		}


		bool verifyOrder(const QSharedPointer<SectionModel>& pSection,
			const QList<QList<ContentItem>>& pOrder)
		{
			const auto& content = pSection->mContentItems;
			qsizetype offset = 0;
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
			mModel.reset(new DiagnosisModel());
		}


		void cleanup()
		{
			mModel.clear();
		}


		void test_newDiagnosisModel()
		{
			QCOMPARE(mModel->rowCount(), mModel->mSections.size());

			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::GENERAL), QCoreApplication::applicationName());
			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::READER), "Card reader"_L1);
			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::NETWORK), "Network"_L1);
			QCOMPARE(mModel->getSectionName(DiagnosisModel::Section::SECURITY), "Antivirus and firewall"_L1);

			QCOMPARE(mModel->mSections[DiagnosisModel::Section::GENERAL]->mContentItems.last().mTitle, "Time of diagnosis"_L1);

			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(0).mTitle, "Paired smartphones"_L1);
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(1).mContent, "No devices paired."_L1);
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(2).mTitle, "Card reader"_L1);
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(3).mContent, "Diagnosis is running..."_L1);
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(4).mTitle, "PC/SC driver information"_L1);
			QCOMPARE(mModel->mSections[DiagnosisModel::Section::READER]->mContentItems.at(5).mContent, "Diagnosis is running..."_L1);

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
			QCOMPARE(mModel->mCardReaderSection.at(1).mContent, "No supported reader found."_L1);

			ReaderInfo defaultInfo;
			ReaderInfo infoEidCard("testInfo"_L1, ReaderManagerPlugInType::PCSC, CardInfo(CardType::EID_CARD));
			ReaderInfo comfortReaderInfo("name"_L1, ReaderManagerPlugInType::REMOTE_IFD, CardInfo(CardType::UNKNOWN));
			comfortReaderInfo.setBasicReader(false);
			const QList<ReaderInfo> readerInfos = {defaultInfo, infoEidCard, comfortReaderInfo};
			mModel->mContext->setReaderInfos(readerInfos);
			mModel->onReaderInfosChanged();

			QCOMPARE(mModel->mCardReaderSection.size(), 4);

			QCOMPARE(mModel->mCardReaderSection.at(0).mTitle, "Connected Card readers"_L1);
			QCOMPARE(mModel->mCardReaderSection.at(1).mTitle, QString());
			QCOMPARE(mModel->mCardReaderSection.at(2).mTitle, "testInfo"_L1);
			QCOMPARE(mModel->mCardReaderSection.at(3).mTitle, "name"_L1);

			QCOMPARE(mModel->mCardReaderSection.at(0).mContent, QString());
			QCOMPARE(mModel->mCardReaderSection.at(1).mContent, "Type: Basic card reader\nCard: not inserted"_L1);
			QCOMPARE(mModel->mCardReaderSection.at(2).mContent, "Type: Basic card reader\nCard: ID card (PA/eAT/eID)\nRetry counter: -1"_L1);
			QCOMPARE(mModel->mCardReaderSection.at(3).mContent, "Type: Standard / comfort card reader\nCard: unknown type"_L1);

			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::READER], {
						mModel->mRemoteDeviceSection,
						mModel->mCardReaderSection,
						mModel->mPcscSection}));
		}


		void test_OnRemoteInfosChanged()
		{
			mModel->onRemoteInfosChanged();
			QCOMPARE(mModel->mRemoteDeviceSection.size(), 2);
			QCOMPARE(mModel->mRemoteDeviceSection.at(0).mTitle, "Paired smartphones"_L1);
			QCOMPARE(mModel->mRemoteDeviceSection.at(1).mContent, "No devices paired."_L1);
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::READER], {
						mModel->mRemoteDeviceSection,
						mModel->mCardReaderSection,
						mModel->mPcscSection}));
		}


		void test_OnPcscInfoChanged()
		{
			const QString version("Version"_L1);
			const DiagnosisContext::ComponentInfo component1("/path/to/component1"_L1, "description1"_L1, "version1"_L1, "vendor1"_L1);
			const DiagnosisContext::ComponentInfo component2("/path/to/component2"_L1, "description2"_L1, "version2"_L1, "vendor2"_L1);
			const DiagnosisContext::ComponentInfo driver1("/path/to/driver1"_L1, "description1"_L1, "version1"_L1, "vendor1"_L1);
			const DiagnosisContext::ComponentInfo driver2("/path/to/driver2"_L1, "description2"_L1, "version2"_L1, "vendor2"_L1);
			QList<DiagnosisContext::ComponentInfo> components = {component1, component2};
			QList<DiagnosisContext::ComponentInfo> drivers = {driver1, driver2};
			mModel->mContext->setPcscInfo(version, components, drivers);
			mModel->onPcscInfoChanged();

			QCOMPARE(mModel->mPcscSection.size(), 4);

			QCOMPARE(mModel->mPcscSection.at(0).mTitle, "PC/SC information"_L1);
			QCOMPARE(mModel->mPcscSection.at(1).mContent, "Version"_L1);
			QCOMPARE(mModel->mPcscSection.at(2).mTitle, "Components"_L1);
			QCOMPARE(mModel->mPcscSection.at(2).mContent, "description1\nVendor: vendor1\nVersion: version1\nFile path: /path/to/component1\ndescription2\nVendor: vendor2\nVersion: version2\nFile path: /path/to/component2"_L1);
			QCOMPARE(mModel->mPcscSection.at(3).mTitle, "Driver"_L1);
			QCOMPARE(mModel->mPcscSection.at(3).mContent, "description1\nVendor: vendor1\nVersion: version1\nFile path: /path/to/driver1\ndescription2\nVendor: vendor2\nVersion: version2\nFile path: /path/to/driver2"_L1);
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

			mModel->mContext->setTimestamp(invalid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampSection.last().mTitle, "Time of diagnosis"_L1);
			QCOMPARE(mModel->mTimestampSection.last().mContent, "Failed to retrieve date & time"_L1);

			mModel->mContext->setTimestamp(valid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampSection.last().mTitle, "Time of diagnosis"_L1);
			QCOMPARE(mModel->mTimestampSection.last().mContent, "12. October 2018, 12:00:00 PM"_L1);
		}


		void test_OnNetworkInfoChanged()
		{
			const QNetworkInterface interface1;
			const QNetworkInterface interface2;
			const QNetworkInterface interface3;
			QList<QNetworkInterface> interfaces = {interface1, interface2, interface3};
			mModel->mContext->setNetworkInterfaces(interfaces);
			mModel->onNetworkInfoChanged();

			QCOMPARE(mModel->mNetworkInterfaceSection.size(), 3);
			for (const auto& item : std::as_const(mModel->mNetworkInterfaceSection))
			{
				QCOMPARE(item.mTitle, QString());
				QCOMPARE(item.mContent, "Hardware address: <Not set>\nNo IP addresses assigned"_L1);
			}

			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::NETWORK], {mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
		}


		void test_OnAntivirusDetectionFailed()
		{
			mModel->onAntivirusDetectionFailed();
			QCOMPARE(mModel->mAntivirusSection.size(), 1);
			QCOMPARE(mModel->mAntivirusSection.at(0).mTitle, "Antivirus information"_L1);
			QCOMPARE(mModel->mAntivirusSection.at(0).mContent, "Antivirus detection failed."_L1);
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnAntivirusInformationChanged()
		{
			mModel->onAntivirusInformationChanged();
			QCOMPARE(mModel->mAntivirusSection.size(), 1);
			QCOMPARE(mModel->mAntivirusSection.at(0).mTitle, "Antivirus information"_L1);
			QCOMPARE(mModel->mAntivirusSection.at(0).mContent, "No Antivirus software detected."_L1);
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));

			const QString& fileContent = getTestData(QStringLiteral("antivir_two_antivirus.txt"));
			mModel->mAntivirusDetection.parseAntivirInfos(fileContent);
			mModel->onAntivirusInformationChanged();

			QCOMPARE(mModel->mAntivirusSection.size(), 3);
			QCOMPARE(mModel->mAntivirusSection.at(0).mTitle, "Antivirus information"_L1);
			QCOMPARE(mModel->mAntivirusSection.at(1).mTitle, "BullGuard Antivirus"_L1);
			QCOMPARE(mModel->mAntivirusSection.at(1).mContent, "Last updated: Fri, 30 Nov 2018 15:04:13 GMT\nExecutable path: C:\\Program Files\\BullGuard Ltd\\BullGuard\\BullGuard.exe"_L1);
			QCOMPARE(mModel->mAntivirusSection.at(2).mTitle, "Windows Defender"_L1);
			QCOMPARE(mModel->mAntivirusSection.at(2).mContent, "Last updated: Mon, 26 Nov 2018 10:34:23 GMT\nExecutable path: windowsdefender://"_L1);
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::SECURITY], {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnFirewallInformationFailed()
		{
			mModel->onFirewallInformationFailed();
			QCOMPARE(mModel->mFirewallSection.size(), 1);
			QCOMPARE(mModel->mFirewallSection.at(0).mTitle, "Firewall information"_L1);
			QCOMPARE(mModel->mFirewallSection.at(0).mContent, "An error occurred while trying to gather firewall information. Please check the log for more information."_L1);
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

			const auto errorString = "Testerror"_L1;

			mModel->mConnectionTest.mIsProxySet = proxySet;
			mModel->mConnectionTest.mPingTestOnProxySuccessful = pingTestOnProxy;
			mModel->mConnectionTest.mConnectionTestWithProxySuccessful = connectionTestOnProxy;
			mModel->mConnectionTest.mConnectionTestWithoutProxySuccessful = connectionTestWithoutProxy;
			if (proxySet && !connectionTestOnProxy)
			{
				mModel->mConnectionTest.mErrorOfConnectionTestWithProxy = errorString;
			}

			mModel->onConnectionTestDone();
			QCOMPARE(mModel->mNetworkConnectionSection.size(), 2);
			QCOMPARE(mModel->mNetworkConnectionSection.at(0).mTitle, "Service addresses"_L1);
			QCOMPARE(mModel->mNetworkConnectionSection.at(1).mTitle, "Proxy information"_L1);

			QString contentString;
			if (proxySet)
			{
				contentString = "Hostname: \nPort: \nType: \nCapabilities: \nPing test to proxy: "_L1;
				contentString += pingTestOnProxy ? "Successful"_L1 : "Failed"_L1;
				contentString += "\nConnection test with proxy: "_L1;
				contentString += connectionTestOnProxy ? "Successful"_L1 : "Failed"_L1;
				if (!connectionTestOnProxy)
				{
					contentString += "\nEncountered error: "_L1;
					contentString += errorString;
				}
			}
			else
			{
				contentString = "No proxy found"_L1;
			}
			contentString += "\nConnection test without proxy: "_L1;
			contentString += connectionTestWithoutProxy ? "Successful"_L1 : "Failed"_L1;

			QCOMPARE(mModel->mNetworkConnectionSection.at(0).mContent, QStringLiteral("Not bound"));
			QCOMPARE(mModel->mNetworkConnectionSection.at(1).mContent, contentString);
			QVERIFY(verifyOrder(mModel->mSections[DiagnosisModel::Section::NETWORK], {mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
		}


		void test_SavePlainText()
		{
			static const QRegularExpression re(QLatin1StringView(R"(^Test_diagnosis_DiagnosisModel\r?
Application\r?
Test_diagnosis_DiagnosisModel\r?
Application Version\r?
Organization\r?
Organization Domain\r?
System\r?
.+\r?
Kernel\r?
.+\r?
Architecture\r?
.+\r?
Device\r?
.+\r?
Qt Version\r?
.+\r?
OpenSSL Version\r?
.+\r?
Time of diagnosis\r?
.+\r?
\r?
\r?
Card reader\r?
Paired smartphones\r?
No devices paired\.\r?
Card reader\r?
Diagnosis is running\.\.\.\r?
PC\/SC driver information\r?
Diagnosis is running\.\.\.\r?
\r?
\r?
Network\r?
(?:\N+\r?\n)*\r?
\r?
Antivirus and firewall\r?
Antivirus information\r?
.+\r?
Firewall information\r?
.+)"));
			QVERIFY(re.match(mModel->getAsPlaintext()).hasMatch());

			QTemporaryFile file;
			QVERIFY(file.open());
			mModel->saveToFile(QUrl::fromLocalFile(file.fileName()));
			QVERIFY(re.match(QString::fromUtf8(file.readAll())).hasMatch());
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisModel)
#include "test_DiagnosisModel.moc"
