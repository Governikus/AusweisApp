/*!
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilterModel.h"

#include "LogHandler.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(gui)


class test_LogFilterModel
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<LogFilterModel> mModel;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void init()
		{
			mModel.reset(new LogFilterModel());

			qCDebug(card) << "test";
			qCWarning(card) << "test";
		}


		void filtering()
		{
			QCOMPARE(mModel->getLevels(), QSet({QString("D"), QString("W")}));
			QCOMPARE(mModel->getSelectedLevels(), QSet<QString>());
			QCOMPARE(mModel->getCategories(), QSet({QString("default"), QString("card")}));
			QCOMPARE(mModel->getSelectedCategories(), QSet<QString>());
			QCOMPARE(mModel->rowCount(), 3);

			QSignalSpy spyLevel(mModel.data(), &LogFilterModel::fireLevelsChanged);
			QSignalSpy spyCategories(mModel.data(), &LogFilterModel::fireCategoriesChanged);

			qCDebug(gui) << "test";
			QCOMPARE(spyLevel.count(), 0);
			QCOMPARE(mModel->getLevels(), QSet({QString("D"), QString("W")}));
			QCOMPARE(mModel->getSelectedLevels(), QSet<QString>());
			QCOMPARE(spyCategories.count(), 1);
			QCOMPARE(mModel->getCategories(), QSet({QString("default"), QString("card"), QString("gui")}));
			QCOMPARE(mModel->getSelectedCategories(), QSet<QString>());
			QCOMPARE(mModel->rowCount(), 4);

			qCInfo(gui) << "test";
			QCOMPARE(spyLevel.count(), 1);
			QCOMPARE(mModel->getLevels(), QSet({QString("D"), QString("W"), QString("I")}));
			QCOMPARE(mModel->getSelectedLevels(), QSet<QString>());
			QCOMPARE(spyCategories.count(), 1);
			QCOMPARE(mModel->getCategories(), QSet({QString("default"), QString("card"), QString("gui")}));
			QCOMPARE(mModel->getSelectedCategories(), QSet<QString>());
			QCOMPARE(mModel->rowCount(), 5);

			mModel->configureLevel(QString("D"), true);
			QCOMPARE(spyLevel.count(), 2);
			QCOMPARE(mModel->getLevels(), QSet({QString("D"), QString("W"), QString("I")}));
			QCOMPARE(mModel->getSelectedLevels(), QSet({QString("D")}));
			QCOMPARE(spyCategories.count(), 1);
			QCOMPARE(mModel->getCategories(), QSet({QString("default"), QString("card"), QString("gui")}));
			QCOMPARE(mModel->getSelectedCategories(), QSet<QString>());
			QCOMPARE(mModel->rowCount(), 3);

			mModel->configureCategory(QString("card"), true);
			QCOMPARE(spyLevel.count(), 2);
			QCOMPARE(mModel->getLevels(), QSet({QString("D"), QString("W"), QString("I")}));
			QCOMPARE(mModel->getSelectedLevels(), QSet({QString("D")}));
			QCOMPARE(spyCategories.count(), 2);
			QCOMPARE(mModel->getCategories(), QSet({QString("default"), QString("card"), QString("gui")}));
			QCOMPARE(mModel->getSelectedCategories(), QSet({QString("card")}));
			QCOMPARE(mModel->rowCount(), 1);

			mModel->configureLevel(QString("D"), false);
			QCOMPARE(spyLevel.count(), 3);
			QCOMPARE(mModel->getLevels(), QSet({QString("D"), QString("W"), QString("I")}));
			QCOMPARE(mModel->getSelectedLevels(), QSet<QString>());
			QCOMPARE(spyCategories.count(), 2);
			QCOMPARE(mModel->getCategories(), QSet({QString("default"), QString("card"), QString("gui")}));
			QCOMPARE(mModel->getSelectedCategories(), QSet({QString("card")}));
			QCOMPARE(mModel->rowCount(), 2);
		}


};

QTEST_GUILESS_MAIN(test_LogFilterModel)
#include "test_LogFilterModel.moc"
