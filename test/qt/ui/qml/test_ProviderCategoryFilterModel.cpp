/*!
 * \brief Unit tests for \ref ProviderCategoryFilterModel
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderCategoryFilterModel.h"

#include <QtTest>

using namespace governikus;

class MockProviderModel
	: public QAbstractListModel
{
	Q_OBJECT
	QMap<int, QVariant> mData;

	public:
		explicit MockProviderModel()
			: mData()
		{
		}


		[[nodiscard]] int rowCount(const QModelIndex& parent) const override
		{
			Q_UNUSED(parent)
			return 1;
		}


		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override
		{
			Q_UNUSED(index)
			return mData.value(role);
		}


		[[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent) const override
		{
			Q_UNUSED(parent)
			QModelIndex index = createIndex(row, column);
			return index;
		}


		[[nodiscard]] QModelIndex parent(const QModelIndex& pIndex) const override
		{
			return pIndex.parent();
		}


		[[nodiscard]] int columnCount(const QModelIndex& parent) const override
		{
			Q_UNUSED(parent)
			return 1;
		}


		void addData(int pRole, const QVariant& pValue)
		{
			mData.insert(pRole, pValue);
		}


};


class test_ProviderCategoryFilterModel
	: public QObject
{
	Q_OBJECT
	QSharedPointer<ProviderCategoryFilterModel> mModel;

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new ProviderCategoryFilterModel());
		}


		void cleanup()
		{
			mModel.reset();
		}


		void test_UpdateSearchString()
		{
			QSignalSpy spy(mModel.data(), &ProviderCategoryFilterModel::fireCriteriaChanged);
			const QString search = QStringLiteral("search");

			mModel->updateSearchString(search);
			QCOMPARE(mModel->getSearchString(), search);
			QCOMPARE(spy.count(), 1);
		}


		void test_CategorySelection()
		{
			QSignalSpy spy(mModel.data(), &ProviderCategoryFilterModel::fireCriteriaChanged);
			const QString category1 = QStringLiteral("CATEGORY");
			const QString category2 = QStringLiteral("testCATEGORY");

			mModel->setCategorySelection(QString());
			QVERIFY(mModel->mSelectedCategories.isEmpty());
			QCOMPARE(spy.count(), 1);

			mModel->setCategorySelection(category1);
			QVERIFY(mModel->mSelectedCategories.contains("category"));
			QCOMPARE(spy.count(), 2);

			mModel->updateCategorySelection(category2, true);
			QVERIFY(mModel->mSelectedCategories.contains("testcategory"));
			QCOMPARE(spy.count(), 3);

			mModel->updateCategorySelection(category2, true);
			QVERIFY(mModel->mSelectedCategories.contains("testcategory"));
			QCOMPARE(spy.count(), 3);

			mModel->updateCategorySelection(category1, false);
			QVERIFY(mModel->mSelectedCategories.contains("testcategory"));
			QVERIFY(!mModel->mSelectedCategories.contains("category"));
			QCOMPARE(spy.count(), 4);
		}


		void test_SortByCategoryFirst()
		{
			mModel->sortByCategoryFirst(true);
			QCOMPARE(mModel->sortRole(), ProviderModel::SORT_ROLE);

			mModel->sortByCategoryFirst(false);
			QCOMPARE(mModel->sortRole(), ProviderModel::SHORTNAME);
		}


		void test_AddAdditionalResultCategories()
		{
			QSignalSpy spy(mModel.data(), &ProviderCategoryFilterModel::fireCriteriaChanged);

			mModel->addAdditionalResultCategories();
			QVERIFY(mModel->getSelectedCategories().isEmpty());
			QCOMPARE(spy.count(), 0);

			const QString category("citizen");
			const QString selected("other");
			const QString searchString("provider1");
			mModel->setCategorySelection(selected);
			mModel->updateSearchString(searchString);
			MockProviderModel model;
			model.addData(Qt::DisplayRole, searchString);
			model.addData(ProviderModel::CATEGORY, category);
			mModel->setSourceModel(&model);

			mModel->addAdditionalResultCategories();
			QVERIFY(mModel->getSelectedCategories().contains(category));
			QCOMPARE(spy.count(), 3);
		}


		void test_ResultCountForFilter()
		{
			const QStringList categories({"citizen"});
			const QString selected("other");
			const QString searchString("provider1");

			QCOMPARE(mModel->resultCountForFilter(categories, searchString), 0);

			mModel->setCategorySelection(selected);
			mModel->updateSearchString(searchString);
			MockProviderModel model;
			model.addData(Qt::DisplayRole, searchString);
			model.addData(ProviderModel::CATEGORY, "citizen");
			mModel->setSourceModel(&model);
			QCOMPARE(mModel->resultCountForFilter(categories, searchString), 1);

			const QStringList unknownCategories({"test category"});
			QCOMPARE(mModel->resultCountForFilter(unknownCategories, searchString), 0);
		}


		void test_FilterAcceptsRow()
		{
			QVERIFY(mModel->filterAcceptsRow(0, QModelIndex()));

			const QString all("all");
			mModel->setCategorySelection(all);
			QVERIFY(mModel->filterAcceptsRow(0, QModelIndex()));

			const QString citizen("citizen");
			mModel->setCategorySelection(citizen);
			MockProviderModel model;
			model.addData(ProviderModel::CATEGORY, citizen);
			mModel->setSourceModel(&model);
			QVERIFY(mModel->filterAcceptsRow(0, QModelIndex()));

			const QString unknownCategory("test category");
			mModel->setCategorySelection(unknownCategory);
			QVERIFY(!mModel->filterAcceptsRow(0, QModelIndex()));

			const QString searchString("search");
			mModel->updateSearchString(searchString);
			QVERIFY(!mModel->filterAcceptsRow(0, QModelIndex()));
		}


};

QTEST_GUILESS_MAIN(test_ProviderCategoryFilterModel)
#include "test_ProviderCategoryFilterModel.moc"
