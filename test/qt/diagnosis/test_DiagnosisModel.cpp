/*!
 * \brief Unit tests for \ref DiagnosisModel
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
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
			const QVector<QSharedPointer<SectionModel>>& pSectionOrder)
		{
			QVector<QVector<QSharedPointer<ContentItem>>> contentOrder;
			for (const auto& section : pSectionOrder)
			{
				contentOrder.append(section->mContentItems);
			}

			return verifyOrder(pSection->mContentItems, contentOrder);
		}


		bool verifyOrder(const QVector<QSharedPointer<ContentItem>>& pSection,
			const QVector<QVector<QSharedPointer<ContentItem>>>& pOrder)
		{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			qsizetype offset = 0;
#else
			int offset = 0;
#endif
			for (const auto& subsection : pOrder)
			{
				const auto& slice = pSection.mid(offset, subsection.size());
				if (slice != subsection)
				{
					return false;
				}
				offset += subsection.size();
			}

			return offset == pSection.size();
		}

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new DiagnosisContext());
			mModel.reset(new DiagnosisModel(mContext));
		}


		void cleanup()
		{
			mModel.clear();
			mContext.clear();
		}


		void test_newDiagnosisModel()
		{
			QCOMPARE(mModel->mContext, mContext);
			QCOMPARE(mModel->mSections.size(), 4);

			QCOMPARE(mModel->mSections.at(0).first, QCoreApplication::applicationName());
			QCOMPARE(mModel->mSections.at(1).first, QString("Card reader"));
			QCOMPARE(mModel->mSections.at(2).first, QString("Network"));
			QCOMPARE(mModel->mSections.at(3).first, QString("Antivirus and firewall"));

			QCOMPARE(mModel->mSections.at(0).second->mContentItems.last(), mModel->mTimestampItem);

			QCOMPARE(mModel->mSections.at(1).second, mModel->mCombinedReaderSection);
			QCOMPARE(mModel->mSections.at(2).second, mModel->mCombinedNetworkSection);
			QCOMPARE(mModel->mSections.at(3).second, mModel->mCombinedAntivirusFirewallSection);

			QCOMPARE(mModel->mSections.at(1).second->mContentItems.at(0)->mTitle, QString("Paired smartphones"));
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.at(1)->mContent, QString("No devices paired."));
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.at(2)->mTitle, QString("Card reader"));
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.at(3)->mContent, QString("Diagnosis is running..."));
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.at(4)->mTitle, QString("PC/SC driver information"));
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.at(5)->mContent, QString("Diagnosis is running..."));
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.mid(0, 2), mModel->mRemoteDeviceSection->mContentItems);
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.mid(2, 2), mModel->mCardReaderSection->mContentItems);
			QCOMPARE(mModel->mSections.at(1).second->mContentItems.mid(4, 2), mModel->mPcscSection->mContentItems);

			QCOMPARE(true, verifyOrder(mModel->mSections.at(1).second,
					{mModel->mRemoteDeviceSection, mModel->mCardReaderSection, mModel->mPcscSection}));
			QCOMPARE(true, verifyOrder(mModel->mSections.at(2).second,
					{mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
			QCOMPARE(true, verifyOrder(mModel->mSections.at(3).second,
					{mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnReaderInfosChanged()
		{
			mModel->onReaderInfosChanged();
			QCOMPARE(mModel->mCardReaderSection->mContentItems.size(), 2);
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(1)->mContent, QString("No supported reader found."));

			ReaderInfo defaultInfo;
			ReaderInfo infoEidCard(QString("testInfo"), ReaderManagerPlugInType::PCSC, CardInfo(CardType::EID_CARD));
			ReaderInfo comfortReaderInfo(QString("name"), ReaderManagerPlugInType::REMOTE_IFD, CardInfo(CardType::UNKNOWN));
			comfortReaderInfo.setBasicReader(false);
			const QVector<ReaderInfo> readerInfos = {defaultInfo, infoEidCard, comfortReaderInfo};
			mContext->setReaderInfos(readerInfos);
			mModel->onReaderInfosChanged();

			QCOMPARE(mModel->mCardReaderSection->mContentItems.size(), 4);

			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(0)->mTitle, QString("Connected Card readers"));
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(1)->mTitle, QString());
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(2)->mTitle, QString("testInfo"));
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(3)->mTitle, QString("name"));

			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(0)->mContent, QString());
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(1)->mContent, QString("Type: Basic card reader\nCard: not inserted"));
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(2)->mContent, QString("Type: Basic card reader\nCard: ID card (PA/eAT/eID)\nRetry counter: -1"));
			QCOMPARE(mModel->mCardReaderSection->mContentItems.at(3)->mContent, QString("Type: Standard / comfort card reader\nCard: unknown type"));

			QCOMPARE(true, verifyOrder(mModel->mCombinedReaderSection->mContentItems, {
						mModel->mRemoteDeviceSection->mContentItems,
						mModel->mCardReaderSection->mContentItems,
						mModel->mPcscSection->mContentItems}));
		}


		void test_OnRemoteInfosChanged()
		{
			mModel->onRemoteInfosChanged();
			QCOMPARE(mModel->mRemoteDeviceSection->mContentItems.size(), 2);
			QCOMPARE(mModel->mRemoteDeviceSection->mContentItems.at(0)->mTitle, QString("Paired smartphones"));
			QCOMPARE(mModel->mRemoteDeviceSection->mContentItems.at(1)->mContent, QString("No devices paired."));
			QCOMPARE(true, verifyOrder(mModel->mCombinedReaderSection->mContentItems, {
						mModel->mRemoteDeviceSection->mContentItems,
						mModel->mCardReaderSection->mContentItems,
						mModel->mPcscSection->mContentItems}));
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

			QCOMPARE(mModel->mPcscSection->mContentItems.size(), 4);

			QCOMPARE(mModel->mPcscSection->mContentItems.at(0)->mTitle, QString("PC/SC information"));
			QCOMPARE(mModel->mPcscSection->mContentItems.at(1)->mContent, QString("Version"));
			QCOMPARE(mModel->mPcscSection->mContentItems.at(2)->mTitle, QString("Components"));
			QCOMPARE(mModel->mPcscSection->mContentItems.at(2)->mContent, QString("description1\nVendor: vendor1\nVersion: version1\nFile path: /path/to/component1\ndescription2\nVendor: vendor2\nVersion: version2\nFile path: /path/to/component2"));
			QCOMPARE(mModel->mPcscSection->mContentItems.at(3)->mTitle, QString("Driver"));
			QCOMPARE(mModel->mPcscSection->mContentItems.at(3)->mContent, QString("description1\nVendor: vendor1\nVersion: version1\nFile path: /path/to/driver1\ndescription2\nVendor: vendor2\nVersion: version2\nFile path: /path/to/driver2"));
			QCOMPARE(true, verifyOrder(mModel->mCombinedReaderSection->mContentItems, {
						mModel->mRemoteDeviceSection->mContentItems,
						mModel->mCardReaderSection->mContentItems,
						mModel->mPcscSection->mContentItems}));
		}


		void test_OnTimestampChanged()
		{
			const QDateTime invalid;
			const QDate date(2018, 10, 12);
			const QTime time(12, 0);
			const QDateTime valid(date, time);

			mContext->setTimestamp(invalid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampItem->mTitle, QString("Time of diagnosis"));
			QCOMPARE(mModel->mTimestampItem->mContent, QString("Failed to retrieve date & time"));

			mContext->setTimestamp(valid);
			mModel->onTimestampChanged();
			QCOMPARE(mModel->mTimestampItem->mTitle, QString("Time of diagnosis"));
			QCOMPARE(mModel->mTimestampItem->mContent, QString("12. October 2018, 12:00:00 PM"));
		}


		void test_OnNetworkInfoChanged()
		{
			const QNetworkInterface interface1;
			const QNetworkInterface interface2;
			const QNetworkInterface interface3;
			QList<QNetworkInterface> interfaces = {interface1, interface2, interface3};
			mContext->setNetworkInterfaces(interfaces);
			mModel->onNetworkInfoChanged();

			QCOMPARE(mModel->mNetworkInterfaceSection->mContentItems.size(), 3);
			for (const auto& item : qAsConst(mModel->mNetworkInterfaceSection->mContentItems))
			{
				QCOMPARE(item->mTitle, QString(""));
				QCOMPARE(item->mContent, QString("Hardware address: <Not set>\nNo IP addresses assigned"));
			}

			QCOMPARE(true, verifyOrder(mModel->mCombinedNetworkSection, {mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
		}


		void test_OnAntivirusDetectionFailed()
		{
			mModel->onAntivirusDetectionFailed();
			QCOMPARE(mModel->mAntivirusSection->mContentItems.size(), 1);
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(0)->mTitle, QString("Antivirus information"));
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(0)->mContent, QString("Antivirus detection failed."));
			QCOMPARE(true, verifyOrder(mModel->mCombinedAntivirusFirewallSection, {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnAntivirusInformationChanged()
		{
			mModel->onAntivirusInformationChanged();
			QCOMPARE(mModel->mAntivirusSection->mContentItems.size(), 1);
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(0)->mTitle, QString("Antivirus information"));
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(0)->mContent, QString("No Antivirus software detected."));
			QCOMPARE(true, verifyOrder(mModel->mCombinedAntivirusFirewallSection, {mModel->mAntivirusSection, mModel->mFirewallSection}));

			const QString& fileContent = getTestData(QStringLiteral("antivir_two_antivirus.txt"));
			mModel->mAntivirusDetection.parseAntivirInfos(fileContent);
			mModel->onAntivirusInformationChanged();

			QCOMPARE(mModel->mAntivirusSection->mContentItems.size(), 3);
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(0)->mTitle, QString("Antivirus information"));
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(1)->mTitle, QString("BullGuard Antivirus"));
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(1)->mContent, QString("Last updated: Fri, 30 Nov 2018 15:04:13 GMT\nExecutable path: C:\\Program Files\\BullGuard Ltd\\BullGuard\\BullGuard.exe"));
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(2)->mTitle, QString("Windows Defender"));
			QCOMPARE(mModel->mAntivirusSection->mContentItems.at(2)->mContent, QString("Last updated: Mon, 26 Nov 2018 10:34:23 GMT\nExecutable path: windowsdefender://"));
			QCOMPARE(true, verifyOrder(mModel->mCombinedAntivirusFirewallSection, {mModel->mAntivirusSection, mModel->mFirewallSection}));
		}


		void test_OnFirewallInformationFailed()
		{
			mModel->onFirewallInformationFailed();
			QCOMPARE(mModel->mFirewallSection->mContentItems.size(), 1);
			QCOMPARE(mModel->mFirewallSection->mContentItems.at(0)->mTitle, QString("Firewall information"));
			QCOMPARE(mModel->mFirewallSection->mContentItems.at(0)->mContent, QString("An error occurred while trying to gather firewall information. Please check the log for more information."));
			QCOMPARE(true, verifyOrder(mModel->mCombinedAntivirusFirewallSection, {mModel->mAntivirusSection, mModel->mFirewallSection}));
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
			QCOMPARE(mModel->mNetworkConnectionSection->mContentItems.size(), 1);
			QCOMPARE(mModel->mNetworkConnectionSection->mContentItems.at(0)->mTitle, QString("Proxy information"));

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

			QCOMPARE(mModel->mNetworkConnectionSection->mContentItems.at(0)->mContent, contentString);
			QCOMPARE(true, verifyOrder(mModel->mCombinedNetworkSection, {mModel->mNetworkConnectionSection, mModel->mNetworkInterfaceSection}));
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisModel)
#include "test_DiagnosisModel.moc"
