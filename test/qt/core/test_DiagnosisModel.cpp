/*!
 * \brief Unit tests for \ref DiagnosisModel
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
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
			QCOMPARE(child2->getChild(2)->getText(), QString("Retry counter: 4"));

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


};

QTEST_GUILESS_MAIN(test_DiagnosisModel)
#include "test_DiagnosisModel.moc"
