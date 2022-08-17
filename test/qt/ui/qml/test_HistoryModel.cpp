/*!
 * \brief Unit tests for \ref HistoryModel
 *
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "ProviderConfiguration.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;

Q_DECLARE_METATYPE(HistoryModel::HistoryRoles)

class test_HistoryModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<HistoryModel> mModel;

	static ProviderConfigurationInfo createProviderInfo(const QStringList& subjectUrls)
	{
		if (subjectUrls.isEmpty())
		{
			return ProviderConfigurationInfo();
		}
		return ProviderConfigurationInfo(
				/* short name  */ QStringLiteral("Provider 1"),
				/* long name  */ QStringLiteral("Provider 1 - long name"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryA"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ subjectUrls);
	}


	static QVector<ProviderConfigurationInfo> setTestProviders(int size)
	{

		QStringList subjectUrls1({QStringLiteral("https://test.test/"), QStringLiteral("https://abc.abc/"), QStringLiteral("abc.abc")});
		QStringList subjectUrls2({QStringLiteral("abc.abc")});
		QStringList subjectUrls3({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")});

		auto& infos = Env::getSingleton<ProviderConfiguration>()->mProviderConfigurationInfos;
		infos.clear();

		switch (size)
		{
			case 1:
				infos.append(createProviderInfo(subjectUrls1));
				break;

			case 3:
				infos.append(createProviderInfo(subjectUrls1));
				infos.append(createProviderInfo(subjectUrls2));
				infos.append(createProviderInfo(subjectUrls3));
		}
		return infos;
	}

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new HistoryModel());
		}


		void cleanup()
		{
			mModel.clear();
		}


		void test_updateConnections_data()
		{
			QTest::addColumn<int>("entriesSize");
			QTest::addColumn<int>("size");

			QTest::newRow("empty") << 0 << 0;
			QTest::newRow("2") << 2 << 4;
			QTest::newRow("10") << 10 << 20;
		}


		void test_updateConnections()
		{
			QFETCH(int, entriesSize);
			QFETCH(int, size);

			HistoryInfo info("SubjectName", "SubjectUrl", "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			QVector<HistoryInfo> entries(entriesSize);
			entries.fill(info);

			Env::getSingleton<AppSettings>()->getHistorySettings().setHistoryInfos(entries);

			mModel->updateConnections();
			QCOMPARE(mModel->mConnections.size(), size);
		}


		void test_DeterminateProviderForEmptyProviderList()
		{
			HistoryInfo info("SubjectName", "SubjectUrl", "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			QCOMPARE(mModel->determineProviderFor(info), ProviderConfigurationInfo());
		}


		void test_DeterminateProviderForProviderListSize1()
		{
			const auto providers = setTestProviders(1);

			HistoryInfo historyInfo1("SubjectName", "", "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			QCOMPARE(mModel->determineProviderFor(historyInfo1), ProviderConfigurationInfo());

			HistoryInfo historyInfo2("SubjectName", QString("https://test.test/"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			QCOMPARE(mModel->determineProviderFor(historyInfo2), providers.at(0));
		}


		void test_DeterminateProviderForProviderListSize3()
		{
			const auto providers = setTestProviders(3);

			HistoryInfo historyInfo1("SubjectName", QString("https://test/"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			HistoryInfo historyInfo2("SubjectName", QString("https://test.test/"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			HistoryInfo historyInfo3("SubjectName", QString("https://abc.abc/"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			HistoryInfo historyInfo4("SubjectName", QString("test.test"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});

			QCOMPARE(mModel->determineProviderFor(historyInfo1), ProviderConfigurationInfo());
			QCOMPARE(mModel->determineProviderFor(historyInfo2), providers.at(0));
			QCOMPARE(mModel->determineProviderFor(historyInfo3), providers.at(0));
			QCOMPARE(mModel->determineProviderFor(historyInfo4), ProviderConfigurationInfo());
		}


		void test_Data_data()
		{
			QTest::addColumn<HistoryModel::HistoryRoles>("role");
			QTest::addColumn<QString>("result");

			QTest::newRow("subject") << HistoryModel::HistoryRoles::SUBJECT << "SubjectName";
			QTest::newRow("purpose") << HistoryModel::HistoryRoles::PURPOSE << "Usage";
			QTest::newRow("termOfUsage") << HistoryModel::HistoryRoles::TERMSOFUSAGE << "TermOfUsage";
			QTest::newRow("requestedData") << HistoryModel::HistoryRoles::REQUESTEDDATA << "Doctoral degree";
			QTest::newRow("writtenData") << HistoryModel::HistoryRoles::WRITTENDATA << "Address";
			QTest::newRow("providerCategory") << HistoryModel::HistoryRoles::PROVIDER_CATEGORY << "CategoryA";
			QTest::newRow("providerShortname") << HistoryModel::HistoryRoles::PROVIDER_SHORTNAME << "Provider 1";
			QTest::newRow("providerLongname") << HistoryModel::HistoryRoles::PROVIDER_LONGNAME << "Provider 1 - long name";
			QTest::newRow("providerLongdescription") << HistoryModel::HistoryRoles::PROVIDER_LONGDESCRIPTION << "Provider description long";
			QTest::newRow("providerAddress") << HistoryModel::HistoryRoles::PROVIDER_ADDRESS << "https://www.homepage.com/";
			QTest::newRow("providerAddressDomain") << HistoryModel::HistoryRoles::PROVIDER_ADDRESS_DOMAIN << "www.homepage.com";
			QTest::newRow("providerHomepage") << HistoryModel::HistoryRoles::PROVIDER_HOMEPAGE << "https://www.homepage.com/";
			QTest::newRow("providerHomepageBase") << HistoryModel::HistoryRoles::PROVIDER_HOMEPAGE_BASE << "www.homepage.com";
			QTest::newRow("providerPhone") << HistoryModel::HistoryRoles::PROVIDER_PHONE << "0421 123456";
			QTest::newRow("providerPhoneCost") << HistoryModel::HistoryRoles::PROVIDER_PHONE_COST << QString();
			QTest::newRow("providerPostalAddress") << HistoryModel::HistoryRoles::PROVIDER_POSTALADDRESS << "Am Fallturm 9\n28359 Bremen";
		}


		void test_Data()
		{
			QFETCH(HistoryModel::HistoryRoles, role);
			QFETCH(QString, result);

			HistoryInfo historyInfo("SubjectName", "https://test.test/", "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"DoctoralDegree", "WriteAddress"});
			QVector<HistoryInfo> infos = {historyInfo};

			setTestProviders(1);

			Env::getSingleton<AppSettings>()->getHistorySettings().setHistoryInfos(infos);
			QModelIndex index = mModel->createIndex(0, 0);

			QCOMPARE(mModel->data(index, role).toString(), result);
		}


		void test_RemoveRows_data()
		{
			QTest::addColumn<int>("oldSize");
			QTest::addColumn<int>("row");
			QTest::addColumn<int>("count");
			QTest::addColumn<int>("newSize");

			QTest::newRow("size5") << 5 << 0 << 5 << 0;
			QTest::newRow("size10") << 10 << 1 << 3 << 7;
			QTest::newRow("size15") << 15 << 8 << 0 << 15;
		}


		void test_RemoveRows()
		{
			QFETCH(int, oldSize);
			QFETCH(int, row);
			QFETCH(int, count);
			QFETCH(int, newSize);

			HistoryInfo historyInfo("SubjectName", "https://www.autentapp.de/bla1", "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			QVector<HistoryInfo> infos(oldSize);
			infos.fill(historyInfo);

			QSignalSpy spyRemove(mModel.data(), &HistoryModel::rowsAboutToBeRemoved);

			Env::getSingleton<AppSettings>()->getHistorySettings().setHistoryInfos(infos);
			if (mModel->removeRows(row, count))
			{
				QCOMPARE(Env::getSingleton<AppSettings>()->getHistorySettings().getHistoryInfos().size(), newSize);
				QList<QVariant> argumentsRemove = spyRemove.takeFirst();
				QCOMPARE(argumentsRemove.at(1).toInt(), row);
				QCOMPARE(argumentsRemove.at(2).toInt(), row + count - 1);
			}
		}


};

QTEST_GUILESS_MAIN(test_HistoryModel)
#include "test_HistoryModel.moc"
