/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ReleaseInformationConfiguration
 */

#include "ReleaseInformationConfiguration.h"

#include "Env.h"
#include "MockReleaseInformation.h"

#include <QSignalSpy>
#include <QTest>

using namespace governikus;

class test_ReleaseInformationConfiguration
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<ReleaseInformationConfiguration> mReleaseInfoConfig;

		int getCurrentInfoUpdateCallcount() const
		{
			return static_cast<const MockReleaseInformation*>(mReleaseInfoConfig->getCurrentInformation().get())->callcountUpdate();
		}


		int getUpdateInfoUpdateCallcount() const
		{
			return static_cast<const MockReleaseInformation*>(mReleaseInfoConfig->getUpdateInformation().get())->callcountUpdate();
		}

	private Q_SLOTS:
		void init()
		{
			Env::setCreator<ReleaseInformation*>(std::function<ReleaseInformation* ()>([] {
					auto* mock = new MockReleaseInformation(VersionNumber("1.2.3"), false);
					return mock;
				}));
			Env::setCreator<ReleaseInformation*>(std::function<ReleaseInformation* (const VersionNumber&, bool)>([](const VersionNumber& pVersion, bool pConsiderOnlyThisVersion){
					auto* mock = new MockReleaseInformation(pVersion, pConsiderOnlyThisVersion);
					return mock;
				}));
			mReleaseInfoConfig.reset(new ReleaseInformationConfiguration());
		}


		void test_update()
		{
			QSignalSpy spyCurrentInformationChanged(mReleaseInfoConfig.data(), &ReleaseInformationConfiguration::fireCurrentInformationChanged);
			QSignalSpy spyUpdateInformationChanged(mReleaseInfoConfig.data(), &ReleaseInformationConfiguration::fireUpdateInformationChanged);

			mReleaseInfoConfig->update();

			QCOMPARE(getCurrentInfoUpdateCallcount(), 1);
			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 0);

			mReleaseInfoConfig->setUpdateVersion(VersionNumber("1.2.4"));

			QCOMPARE(getUpdateInfoUpdateCallcount(), 1);
			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 1);

			mReleaseInfoConfig->update();

			QCOMPARE(getCurrentInfoUpdateCallcount(), 2);
			QCOMPARE(getUpdateInfoUpdateCallcount(), 2);
			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 1);
		}


		void test_updateIfNeeded()
		{
			QSignalSpy spyCurrentInformationChanged(mReleaseInfoConfig.data(), &ReleaseInformationConfiguration::fireCurrentInformationChanged);
			QSignalSpy spyUpdateInformationChanged(mReleaseInfoConfig.data(), &ReleaseInformationConfiguration::fireUpdateInformationChanged);

			mReleaseInfoConfig->updateIfNeeded();

			QCOMPARE(getCurrentInfoUpdateCallcount(), 1);
			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 0);

			mReleaseInfoConfig->setUpdateVersion(VersionNumber("1.2.4"));
			QCOMPARE(getUpdateInfoUpdateCallcount(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 1);

			mReleaseInfoConfig->updateIfNeeded();

			QCOMPARE(getCurrentInfoUpdateCallcount(), 1);
			QCOMPARE(getUpdateInfoUpdateCallcount(), 1);
			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 1);
		}


		void test_reload()
		{
			QSignalSpy spyCurrentInformationChanged(mReleaseInfoConfig.data(), &ReleaseInformationConfiguration::fireCurrentInformationChanged);
			QSignalSpy spyUpdateInformationChanged(mReleaseInfoConfig.data(), &ReleaseInformationConfiguration::fireUpdateInformationChanged);

			mReleaseInfoConfig->update();

			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 0);

			mReleaseInfoConfig->setUpdateVersion(VersionNumber("1.4.2"));

			QCOMPARE(spyCurrentInformationChanged.count(), 1);
			QCOMPARE(spyUpdateInformationChanged.count(), 1);

			const auto currentInfoBefore = mReleaseInfoConfig->getCurrentInformation();
			const auto updateInfoBefore = mReleaseInfoConfig->getUpdateInformation();

			mReleaseInfoConfig->reload();

			QVERIFY(currentInfoBefore.get() != mReleaseInfoConfig->getCurrentInformation().get());
			QVERIFY(updateInfoBefore.get() != mReleaseInfoConfig->getUpdateInformation().get());

			QCOMPARE(spyCurrentInformationChanged.count(), 2);
			QCOMPARE(spyUpdateInformationChanged.count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_ReleaseInformationConfiguration)
#include "test_ReleaseInformationConfiguration.moc"
