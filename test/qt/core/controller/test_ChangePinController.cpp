/*!
 * test_ChangePinController.h
 *
 * \brief Test for the Change PIN functionality.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QScopedPointer>
#include <QSharedPointer>
#include <QtTest>

#include "PersoSimController.h"
#include "ReaderManager.h"
#include "context/AuthContext.h"
#include "controller/ChangePinController.h"
#include "testMacros.h"


using namespace governikus;


class test_ChangePinController
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<PersoSimController> mPersoSim;
		QSharedPointer<ChangePinContext> mChangePinContext;
		QScopedPointer<WorkflowController> mChangePinController;

	private Q_SLOTS:
		void initTestCase()
		{
			SKIP_IF_PERSOSIM_DISABLED();

			mPersoSim.reset(new PersoSimController);
			QVERIFY(mPersoSim->init());

			ReaderManager::getInstance().init();
		}


		void cleanupTestCase()
		{
			ReaderManager::getInstance().shutdown();
		}


		void cleanup()
		{
			mChangePinController.reset();
			mChangePinContext.reset();
		}


		void testSuccess()
		{
			// set up model
			//ChangePinParameters parameters(getPersoSimReaderName(), "123456", "123456");
			//mChangePinModel.reset(new ChangePinModel(parameters));

			// create and run controller
			mChangePinController.reset(new ChangePinController(mChangePinContext));

			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);

			mChangePinController->run();

			QVERIFY(controllerFinishedSpy.wait(10000));

			//QCOMPARE(mChangePinModel->getResult().getMajor(), Result::Major::Ok);
		}


		void testWrongPin()
		{
			// set up model
			//ChangePinParameters parameters(getPersoSimReaderName(), "111111", "111111");
			//mChangePinModel.reset(new ChangePinModel(parameters));

			// create and run controller
			mChangePinController.reset(new ChangePinController(mChangePinContext));

			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);

			mChangePinController->run();

			QVERIFY(controllerFinishedSpy.wait(10000));

			//QCOMPARE(mChangePinModel->getResult().getMajor(), Result::Major::Error);
			//QCOMPARE(mChangePinModel->getResult().getMinor(), Result::Minor::AL_Internal_Error);
		}


	private:
		QString getPersoSimReaderName() const
		{
			QString readerName;
			const auto& infos = ReaderManager::getInstance().getReaderInfos();
			for (const ReaderInfo& readerInfo : infos)
			{
				if (readerInfo.getName().startsWith(QLatin1String("PersoSim")))
				{
					XVERIFY2(readerName.isEmpty(), "multiple PersoSim readers", QString());
					readerName = readerInfo.getName();
				}
			}

			XVERIFY2(!readerName.isEmpty(), "no PersoSim reader", QString());
			return readerName;
		}


};

QTEST_GUILESS_MAIN(test_ChangePinController)
#include "test_ChangePinController.moc"
