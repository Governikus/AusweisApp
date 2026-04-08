/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
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
			LogModel logModel;
			logModel.setSource(QString());
			LogFilterModel model;
			model.setSourceModel(&logModel);

			qCDebug(card) << "test";
			qCWarning(card) << "test";

			QTRY_COMPARE(model.getLevels(), QStringList({"D"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QStringList());
			QCOMPARE(model.getCategories(), QStringList({"card"_L1}));
			QCOMPARE(model.getSelectedCategories(), QStringList());
			QCOMPARE(model.rowCount(), 2);

			qApp->processEvents();
			QSignalSpy spyLevel(&model, &LogFilterModel::fireLevelsChanged);
			QSignalSpy spyCategories(&model, &LogFilterModel::fireCategoriesChanged);

			qCDebug(gui) << "test";

			QTRY_COMPARE(spyLevel.count(), 0);
			QCOMPARE(model.getLevels(), QStringList({"D"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QStringList());
			QTRY_COMPARE(spyCategories.count(), 1);
			QCOMPARE(model.getCategories(), QStringList({"card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QStringList());
			QCOMPARE(model.rowCount(), 3);

			qCInfo(gui) << "test";

			QTRY_COMPARE(spyLevel.count(), 1);
			QCOMPARE(model.getLevels(), QStringList({"D"_L1, "I"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QStringList());
			QTRY_COMPARE(spyCategories.count(), 1);
			QCOMPARE(model.getCategories(), QStringList({"card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QStringList());
			QCOMPARE(model.rowCount(), 4);

			model.configureLevel("D"_L1, true);

			QTRY_COMPARE(spyLevel.count(), 2);
			QCOMPARE(model.getLevels(), QStringList({"D"_L1, "I"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QStringList({"D"_L1}));
			QTRY_COMPARE(spyCategories.count(), 1);
			QCOMPARE(model.getCategories(), QStringList({"card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QStringList());
			QCOMPARE(model.rowCount(), 2);

			model.configureCategory("card"_L1, true);

			QTRY_COMPARE(spyLevel.count(), 2);
			QCOMPARE(model.getLevels(), QStringList({"D"_L1, "I"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QStringList({"D"_L1}));
			QTRY_COMPARE(spyCategories.count(), 2);
			QCOMPARE(model.getCategories(), QStringList({"card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QStringList({"card"_L1}));
			QCOMPARE(model.rowCount(), 1);

			model.configureLevel("D"_L1, false);

			QTRY_COMPARE(spyLevel.count(), 3);
			QCOMPARE(model.getLevels(), QStringList({"D"_L1, "I"_L1, "W"_L1}));
			QCOMPARE(model.getSelectedLevels(), QStringList());
			QTRY_COMPARE(spyCategories.count(), 2);
			QCOMPARE(model.getCategories(), QStringList({"card"_L1, "gui"_L1}));
			QCOMPARE(model.getSelectedCategories(), QStringList({"card"_L1}));
			QCOMPARE(model.rowCount(), 2);
		}


};

QTEST_GUILESS_MAIN(test_LogFilterModel)
#include "test_LogFilterModel.moc"
