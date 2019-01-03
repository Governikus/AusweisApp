/*!
 * \brief Unit tests for \ref ProviderCategoryFilterModel
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderCategoryFilterModel.h"

#include <QtTest>


using namespace governikus;


class test_ProviderCategoryFilterModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_UpdateSearchString()
		{
			ProviderCategoryFilterModel model;
			QSignalSpy spy(&model, &ProviderCategoryFilterModel::fireCriteriaChanged);
			const QString search = QStringLiteral("search");

			model.updateSearchString(search);
			QCOMPARE(model.getSearchString(), search);
			QCOMPARE(spy.count(), 1);
		}


		void test_CategorySelection()
		{
			ProviderCategoryFilterModel model;
			QSignalSpy spy(&model, &ProviderCategoryFilterModel::fireCriteriaChanged);
			const QString category1 = QStringLiteral("CATEGORY");
			const QString category2 = QStringLiteral("testCATEGORY");

			model.setCategorySelection(QString());
			QVERIFY(model.mSelectedCategories.isEmpty());
			QCOMPARE(spy.count(), 1);

			model.setCategorySelection(category1);
			QVERIFY(model.mSelectedCategories.contains("category"));
			QCOMPARE(spy.count(), 2);

			model.updateCategorySelection(category2, true);
			QVERIFY(model.mSelectedCategories.contains("testcategory"));
			QCOMPARE(spy.count(), 3);

			model.updateCategorySelection(category2, true);
			QVERIFY(model.mSelectedCategories.contains("testcategory"));
			QCOMPARE(spy.count(), 3);

			model.updateCategorySelection(category1, false);
			QVERIFY(model.mSelectedCategories.contains("testcategory"));
			QVERIFY(!model.mSelectedCategories.contains("category"));
			QCOMPARE(spy.count(), 4);
		}


		void test_SortByCategoryFirst()
		{
			ProviderCategoryFilterModel model;

			model.sortByCategoryFirst(true);
			QCOMPARE(model.sortRole(), ProviderModel::SORT_ROLE);

			model.sortByCategoryFirst(false);
			QCOMPARE(model.sortRole(), ProviderModel::SHORTNAME);
		}


};

QTEST_GUILESS_MAIN(test_ProviderCategoryFilterModel)
#include "test_ProviderCategoryFilterModel.moc"
