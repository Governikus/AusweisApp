/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderCategoryFilterModel.h"


using namespace governikus;

namespace
{
const QStringList& getCategories()
{
	static QStringList cats({QStringLiteral("citizen"), QStringLiteral("insurance"), QStringLiteral("finance"), QStringLiteral("other")});
	return cats;
}


} // namespace


QString ProviderCategoryFilterModel::getSearchString() const
{
	return mSearchString;
}


void ProviderCategoryFilterModel::updateSearchString(const QString& pSearchString)
{
	const QString& newSearchString = pSearchString.trimmed();
	if (mSearchString != newSearchString)
	{
		mSearchString = newSearchString;
		invalidateFilter();
		Q_EMIT fireCriteriaChanged();
	}
}


QStringList ProviderCategoryFilterModel::getSelectedCategories() const
{
	return mSelectedCategories.toList();
}


int ProviderCategoryFilterModel::getAdditionalResultCount() const
{
	int results = 0;
	for (const QString& p : getCategories())
	{
		results += matchesForExcludedCategory(p);
	}
	return results;
}


int ProviderCategoryFilterModel::matchesForExcludedCategory(const QString& pCategory) const
{
	if (mSearchString.isEmpty() || mSelectedCategories.isEmpty() || mSelectedCategories.contains(pCategory))
	{
		return 0;
	}

	QAbstractItemModel* const model = sourceModel();
	const int count = model->rowCount();
	int matchCount = 0;
	for (int sourceRow = 0; sourceRow < count; ++sourceRow)
	{
		const QModelIndex idx = model->index(sourceRow, 0, QModelIndex());
		const QString dt = model->data(idx, Qt::DisplayRole).toString();
		if (!dt.contains(mSearchString, Qt::CaseInsensitive))
		{
			continue;
		}

		if (pCategory.toLower() == model->data(idx, ProviderModel::CATEGORY).toString().toLower())
		{
			matchCount++;
		}
	}

	return matchCount;
}


bool ProviderCategoryFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const
{
	QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const QModelIndex idx = model->index(pSourceRow, 0, pSourceParent);

	if (!mSearchString.isEmpty())
	{
		const QString dt = model->data(idx, Qt::DisplayRole).toString();
		if (!dt.contains(mSearchString, Qt::CaseInsensitive))
		{
			return false;
		}
	}

	return mSelectedCategories.isEmpty() || mSelectedCategories.contains(QStringLiteral("all")) ||
		   mSelectedCategories.contains(model->data(idx, ProviderModel::CATEGORY).toString().toLower());
}


ProviderCategoryFilterModel::ProviderCategoryFilterModel() :
	mProviderModel()
{
	QSortFilterProxyModel::setSourceModel(&mProviderModel);

	QSortFilterProxyModel::sort(0);
	sortByCategoryFirst(false);
	setSortCaseSensitivity(Qt::CaseInsensitive);
}


ProviderCategoryFilterModel::~ProviderCategoryFilterModel()
{
}


void ProviderCategoryFilterModel::sortByCategoryFirst(bool pEnabled)
{
	setSortRole(pEnabled ? ProviderModel::SORT_ROLE : ProviderModel::SHORTNAME);
}


void ProviderCategoryFilterModel::setCategorySelection(const QString& pCategory)
{
	mSelectedCategories.clear();

	if (!pCategory.isEmpty())
	{
		mSelectedCategories.insert(pCategory.toLower());
	}
	invalidateFilter();
	Q_EMIT fireCriteriaChanged();
}


void ProviderCategoryFilterModel::updateCategorySelection(const QString& pCategory, bool pSelected)
{
	const int categoryCount = mSelectedCategories.count();
	if (pSelected)
	{
		mSelectedCategories.insert(pCategory.toLower());
	}
	else
	{
		mSelectedCategories.remove(pCategory.toLower());
	}

	if (mSelectedCategories.count() != categoryCount)
	{
		invalidateFilter();
		Q_EMIT fireCriteriaChanged();
	}
}


void ProviderCategoryFilterModel::addAdditionalResultCategories()
{
	bool needUpdate = false;
	for (const QString& p : getCategories())
	{
		if (matchesForExcludedCategory(p) > 0)
		{
			needUpdate = true;
			mSelectedCategories += p;
		}
	}
	if (needUpdate)
	{
		invalidateFilter();
		Q_EMIT fireCriteriaChanged();
	}
}
