/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderCategoryFilterModel.h"


using namespace governikus;

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
	return mSelectedCategories;
}


int ProviderCategoryFilterModel::getAdditionalResultCount() const
{
	if (mSearchString.isEmpty() || mSelectedCategories.isEmpty())
	{
		return 0;
	}

	const auto excludedCategories = getFilteredProviderCategories();
	if (excludedCategories.isEmpty())
	{
		return 0;
	}

	return resultCountForFilter(excludedCategories, mSearchString);
}


QStringList ProviderCategoryFilterModel::getFilteredProviderCategories() const
{
	auto filteredCats = ProviderModel::getProviderCategories();
	filteredCats.removeOne(QStringLiteral("all"));
	for (const auto& cat : std::as_const(mSelectedCategories))
	{
		filteredCats.removeOne(cat);
	}
	return filteredCats;
}


int ProviderCategoryFilterModel::resultCountForFilter(const QStringList& pCategories, const QString& pSearchString) const
{
	const QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const int rowCount = model->rowCount();

	int matchCount = 0;
	for (int sourceRow = 0; sourceRow < rowCount; ++sourceRow)
	{
		if (rowMatchesFilter(sourceRow, QModelIndex(), pCategories, pSearchString, false))
		{
			matchCount++;
		}
	}

	return matchCount;
}


bool ProviderCategoryFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const
{
	return rowMatchesFilter(pSourceRow, pSourceParent, mSelectedCategories, mSearchString, mProviderModel.getIncludeCategories());
}


bool ProviderCategoryFilterModel::rowMatchesFilter(int pSourceRow, const QModelIndex& pSourceParent, const QStringList& pSelectedCategories, const QString& pSearchString, bool pMatchCategoryType) const
{
	const QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const QModelIndex idx = model->index(pSourceRow, 0, pSourceParent);

	const QString providerCategory = model->data(idx, ProviderModel::CATEGORY).toString().toLower();
	bool isRowInSelectedCategory = pSelectedCategories.isEmpty() ||
			pSelectedCategories.contains(QLatin1String("all")) ||
			pSelectedCategories.contains(providerCategory);

	if (!isRowInSelectedCategory)
	{
		return false;
	}

	const QString type = model->data(idx, ProviderModel::TYPE).toString();
	if (type == QLatin1String("category"))
	{
		if (!pMatchCategoryType)
		{
			return false;
		}

		if (!pSearchString.isEmpty() && providerCategory == QLatin1String("all"))
		{
			return false;
		}

		return resultCountForFilter({providerCategory}, pSearchString) > 0;
	}

	if (pSearchString.isEmpty())
	{
		return !pMatchCategoryType;
	}

	const QString display = model->data(idx, Qt::DisplayRole).toString();
	const QString shortname = model->data(idx, ProviderModel::SHORTNAME).toString();
	const QString longname = model->data(idx, ProviderModel::LONGNAME).toString();
	const QString address = model->data(idx, ProviderModel::ADDRESS).toString();
	const QString homepage = model->data(idx, ProviderModel::HOMEPAGE).toString();
	const QString longdescription = model->data(idx, ProviderModel::LONGDESCRIPTION).toString();

	return display.contains(pSearchString, Qt::CaseInsensitive) ||
		   shortname.contains(pSearchString, Qt::CaseInsensitive) ||
		   longname.contains(pSearchString, Qt::CaseInsensitive) ||
		   address.contains(pSearchString, Qt::CaseInsensitive) ||
		   homepage.contains(pSearchString, Qt::CaseInsensitive) ||
		   longdescription.contains(pSearchString, Qt::CaseInsensitive);
}


ProviderCategoryFilterModel::ProviderCategoryFilterModel() :
	mProviderModel()
{
	QSortFilterProxyModel::setSourceModel(&mProviderModel);

	QSortFilterProxyModel::sort(0);
	sortByCategoryFirst(false);
	setSortCaseSensitivity(Qt::CaseInsensitive);
}


void ProviderCategoryFilterModel::setIncludeCategoriesInModel(bool pIncludeCategories)
{
	mProviderModel.setIncludeCategories(pIncludeCategories);
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
		mSelectedCategories += pCategory.toLower();
	}
	invalidateFilter();
	Q_EMIT fireCriteriaChanged();
}


void ProviderCategoryFilterModel::updateCategorySelection(const QString& pCategory, bool pSelected)
{
	const auto categoryCount = mSelectedCategories.count();
	if (pSelected)
	{
		mSelectedCategories += pCategory.toLower();
	}
	else
	{
		mSelectedCategories.removeAll(pCategory.toLower());
	}

	mSelectedCategories.removeDuplicates();
	if (mSelectedCategories.count() != categoryCount)
	{
		invalidateFilter();
		Q_EMIT fireCriteriaChanged();
	}
}


void ProviderCategoryFilterModel::addAdditionalResultCategories()
{
	bool filterChanged = false;
	const auto excludedCategories = getFilteredProviderCategories();
	for (const QString& category : excludedCategories)
	{
		if (resultCountForFilter({category}, mSearchString) > 0)
		{
			filterChanged = true;
			mSelectedCategories += category;
		}
	}

	if (filterChanged)
	{
		invalidateFilter();
		Q_EMIT fireCriteriaChanged();
	}
}
