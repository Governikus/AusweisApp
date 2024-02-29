/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilterModel.h"

#include "LogHandler.h"

#include <QDebug>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)
Q_DECLARE_LOGGING_CATEGORY(gui)


class test_LogFilterModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void filtering()
		{
			LogFilterModel model;

			qCDebug(card) << "test";
			qCWarning(card) << "test";

			QCOMPARE(model.getLevels(), QSet<QString>({"D"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QSet<QString>());
			QCOMPARE(model.getCategories(), QSet<QString>({"default"_L1, "card"_L1}));
			QCOMPARE(model.getSelectedCategories(), QSet<QString>());
			QCOMPARE(model.rowCount(), 3);

			QSignalSpy spyLevel(&model, &LogFilterModel::fireLevelsChanged);
			QSignalSpy spyCategories(&model, &LogFilterModel::fireCategoriesChanged);

			qCDebug(gui) << "test";
			QCOMPARE(spyLevel.count(), 0);
			QCOMPARE(model.getLevels(), QSet<QString>({"D"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QSet<QString>());
			QCOMPARE(spyCategories.count(), 1);
			QCOMPARE(model.getCategories(), QSet<QString>({"default"_L1, "card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QSet<QString>());
			QCOMPARE(model.rowCount(), 4);

			qCInfo(gui) << "test";
			QCOMPARE(spyLevel.count(), 1);
			QCOMPARE(model.getLevels(), QSet<QString>({"D"_L1, "W"_L1, "I"_L1}));
			QCOMPARE(model.getSelectedLevels(), QSet<QString>());
			QCOMPARE(spyCategories.count(), 1);
			QCOMPARE(model.getCategories(), QSet<QString>({"default"_L1, "card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QSet<QString>());
			QCOMPARE(model.rowCount(), 5);

			model.configureLevel("D"_L1, true);
			QCOMPARE(spyLevel.count(), 2);
			QCOMPARE(model.getLevels(), QSet<QString>({"D"_L1, "W"_L1, "I"_L1}));
			QCOMPARE(model.getSelectedLevels(), QSet<QString>({"D"_L1}));
			QCOMPARE(spyCategories.count(), 1);
			QCOMPARE(model.getCategories(), QSet<QString>({"default"_L1, "card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QSet<QString>());
			QCOMPARE(model.rowCount(), 3);

			model.configureCategory("card"_L1, true);
			QCOMPARE(spyLevel.count(), 2);
			QCOMPARE(model.getLevels(), QSet<QString>({"D"_L1, "W"_L1, "I"_L1}));
			QCOMPARE(model.getSelectedLevels(), QSet<QString>({"D"_L1}));
			QCOMPARE(spyCategories.count(), 2);
			QCOMPARE(model.getCategories(), QSet<QString>({"default"_L1, "card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QSet<QString>({"card"_L1}));
			QCOMPARE(model.rowCount(), 1);

			model.configureLevel("D"_L1, false);
			QCOMPARE(spyLevel.count(), 3);
			QCOMPARE(model.getLevels(), QSet<QString>({"D"_L1, "W"_L1, "I"_L1}));
			QCOMPARE(model.getSelectedLevels(), QSet<QString>());
			QCOMPARE(spyCategories.count(), 2);
			QCOMPARE(model.getCategories(), QSet<QString>({"default"_L1, "card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QSet<QString>({"card"_L1}));
			QCOMPARE(model.rowCount(), 2);
		}


};

QTEST_GUILESS_MAIN(test_LogFilterModel)
#include "test_LogFilterModel.moc"
