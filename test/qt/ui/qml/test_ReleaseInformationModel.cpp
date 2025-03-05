/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformationModel.h"

#include "Env.h"
#include "MockReleaseInformation.h"
#include "ReleaseInformationConfiguration.h"

#include <QSharedPointer>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_ReleaseInformationModel
	: public QObject
{
	Q_OBJECT

	QScopedPointer<ReleaseInformationModel> mModel;
	QScopedPointer<QSignalSpy> mSpyCurrentInformationChanged;
	QScopedPointer<QSignalSpy> mSpyUpdateInformationChanged;
	QScopedPointer<ReleaseInformationConfiguration> mReleaseInfoConfig;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<ReleaseInformation*>(std::function<ReleaseInformation* ()>([] {
						return new MockReleaseInformation(VersionNumber("1.2.3"_L1), false);
					}));
			Env::setCreator<ReleaseInformation*>(std::function<ReleaseInformation* (const VersionNumber&, bool)>([](const VersionNumber& pVersion, bool pConsiderOnlyThisVersion){
						return new MockReleaseInformation(pVersion, pConsiderOnlyThisVersion);
					}));
		}


		void init()
		{
			mReleaseInfoConfig.reset(new ReleaseInformationConfiguration());
			Env::set(ReleaseInformationConfiguration::staticMetaObject, mReleaseInfoConfig.get());

			mModel.reset(new ReleaseInformationModel());

			mSpyCurrentInformationChanged.reset(new QSignalSpy(mModel.data(), &ReleaseInformationModel::fireCurrentInformationChanged));
			mSpyUpdateInformationChanged.reset(new QSignalSpy(mModel.data(), &ReleaseInformationModel::fireUpdateInformationChanged));
		}


		void cleanupTestCase()
		{
			Env::clear();
		}


		void allowRetry()
		{
			QCOMPARE(mModel->allowRetry(), true);
			mModel->update();
			QCOMPARE(mModel->allowRetry(), false);
		}


		void checkInitialState()
		{
			QCOMPARE(mModel->allowRetry(), true);
			QCOMPARE(mSpyCurrentInformationChanged->count(), 0);
			QCOMPARE(mSpyUpdateInformationChanged->count(), 0);
			QCOMPARE(mModel->getCurrentRelease()->rowCount(), 1);
			QCOMPARE(mModel->getUpdateRelease()->rowCount(), 1);
		}


		void ensureChangedSignalOnCurrentReleaseInfoChanged()
		{
			QCOMPARE(mModel->getCurrentRelease()->rowCount(), 1);

			mModel->update();

			QCOMPARE(mSpyCurrentInformationChanged->count(), 1);
			QCOMPARE(mSpyUpdateInformationChanged->count(), 0);
			QCOMPARE(mModel->getCurrentRelease()->rowCount(), 11);
			QCOMPARE(mModel->getUpdateRelease()->rowCount(), 1);
		}


		void ensureSignalOnUpdateReleaseInfoChanged()
		{
			QCOMPARE(mModel->getUpdateRelease()->rowCount(), 1);

			mReleaseInfoConfig->setUpdateVersion(VersionNumber("1.2.4"_L1));

			QCOMPARE(mSpyUpdateInformationChanged->count(), 1);
			QCOMPARE(mModel->getUpdateRelease()->rowCount(), 11);
		}


		void ensureSignalOnTranslationChanged()
		{
			mModel->onTranslationChanged();

			QCOMPARE(mSpyCurrentInformationChanged->count(), 1);

			mReleaseInfoConfig->setUpdateVersion(VersionNumber("1.2.4"_L1));

			QCOMPARE(mSpyUpdateInformationChanged->count(), 1);

			mModel->onTranslationChanged();

			QCOMPARE(mSpyUpdateInformationChanged->count(), 2);
			QCOMPARE(mSpyCurrentInformationChanged->count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_ReleaseInformationModel)
#include "test_ReleaseInformationModel.moc"
