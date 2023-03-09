/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ProviderNameFilterModel
 */

#include "ProviderNameFilterModel.h"

#include "AppSettings.h"
#include "HistoryModel.h"
#include "ProviderConfiguration.h"
#include "ResourceLoader.h"
#include "VolatileSettings.h"

#include <QtTest>


using namespace governikus;


class test_ProviderNameFilterModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<ProviderNameFilterModel> mModel;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mModel.reset(new ProviderNameFilterModel());
		}


		void cleanup()
		{
			mModel.clear();
		}


		void test_FilterAcceptsRow()
		{
			SDK_MODE(false);
			QVERIFY(!mModel->filterAcceptsRow(0, QModelIndex()));

			HistoryModel model;
			mModel->setSourceModel(&model);

			HistoryInfo historyInfo1("SubjectName", QString("https://www.ausweisapp.bund.de/aa2/download"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			HistoryInfo historyInfo2("SubjectName", QString("https://test.de"), "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			Env::getSingleton<AppSettings>()->getHistorySettings().addHistoryInfo(historyInfo1);
			Env::getSingleton<AppSettings>()->getHistorySettings().addHistoryInfo(historyInfo2);
			const QString providerAddress("https://www.ausweisapp.bund.de/aa2/download");
			mModel->setProviderAddress(providerAddress);

			QVERIFY(!mModel->filterAcceptsRow(0, QModelIndex()));
			QVERIFY(mModel->filterAcceptsRow(1, QModelIndex()));
		}


		void test_SetProviderAddress()
		{
			const QString invalidProviderAddress("https://test.de/");
			QTest::ignoreMessage(QtWarningMsg, "Cannot select provider with address \"https://test.de/\"");
			mModel->setProviderAddress(invalidProviderAddress);
			QCOMPARE(mModel->mProvider.getAddress(), QString());

			const QString validProviderAddress("https://www.ausweisapp.bund.de/aa2/download");
			mModel->setProviderAddress(validProviderAddress);
			QCOMPARE(mModel->mProvider.getAddress(), validProviderAddress);
		}


};

QTEST_GUILESS_MAIN(test_ProviderNameFilterModel)
#include "test_ProviderNameFilterModel.moc"
