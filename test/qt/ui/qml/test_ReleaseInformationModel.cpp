/*!
 * \brief Unit tests for \ref ReleaseInformationModel
 *
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformationModel.h"

#include "MockReleaseInformation.h"

#include <QSharedPointer>
#include <QtTest>

using namespace governikus;

class test_ReleaseInformationModel
	: public QObject
{
	Q_OBJECT

	QScopedPointer<ReleaseInformationModel> mModel;

	private Q_SLOTS:
		void init()
		{
			Env::setCreator<ReleaseInformation*>(std::function<ReleaseInformation* ()>([] {
						auto* mock = new MockReleaseInformation(VersionNumber("1.2.3."), false);
						return mock;
					}));
			Env::setCreator<ReleaseInformation*>(std::function<ReleaseInformation* (const VersionNumber&, bool)>([&](const VersionNumber& pVersion, bool pConsiderOnlyThisVersion){
						auto* mock = new MockReleaseInformation(pVersion, pConsiderOnlyThisVersion);
						return mock;
					}));
			mModel.reset(new ReleaseInformationModel());
		}


		void test_requiresInitialUpdate()
		{
			QCOMPARE(mModel->requiresInitialUpdate(), true);
			mModel->update();
			QCOMPARE(mModel->requiresInitialUpdate(), false);
		}


		void test_allowRetry()
		{
			QCOMPARE(mModel->allowRetry(), true);
			mModel->update();
			QCOMPARE(mModel->allowRetry(), false);
		}


		void test_currentModel()
		{
			connect(mModel.data(), &ReleaseInformationModel::fireCurrentInformationChanged, this, [this](){
						const auto textModel = mModel->getCurrentRelease();
						QCOMPARE(textModel->rowCount(), 11);
					});

			QSignalSpy spyInformationChanged(mModel.data(), &ReleaseInformationModel::fireCurrentInformationChanged);
			const auto textModel = mModel->getCurrentRelease();
			QCOMPARE(textModel->rowCount(), 1);
			mModel->update();
			QVERIFY(spyInformationChanged.count() == 1);
		}


		void test_updateModel()
		{
			connect(mModel.data(), &ReleaseInformationModel::fireUpdateInformationChanged, this, [this](){
						const auto textModel = mModel->getUpdateRelease();
						QCOMPARE(textModel->rowCount(), 11);
					});

			QSignalSpy spyInformationChanged(mModel.data(), &ReleaseInformationModel::fireUpdateInformationChanged);
			const auto textModel = mModel->getUpdateRelease();
			QCOMPARE(textModel->rowCount(), 1);
			mModel->setUpdateVersion(VersionNumber("1.2.4"));
			QVERIFY(spyInformationChanged.count() == 1);
		}


};

QTEST_GUILESS_MAIN(test_ReleaseInformationModel)
#include "test_ReleaseInformationModel.moc"
