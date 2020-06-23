/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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
	return mSelectedCategories.values();
}


int ProviderCategoryFilterModel::getAdditionalResultCount() const
{
	if (mSearchString.isEmpty() || mSelectedCategories.isEmpty())
	{
		return 0;
	}

	auto excludedCategories = ProviderModel::getProviderCategories() - mSelectedCategories;
	if (excludedCategories.isEmpty())
	{
		return 0;
	}

	return resultCountForFilter(excludedCategories, mSearchString);
}


int ProviderCategoryFilterModel::resultCountForFilter(const QSet<QString>& pCategories, const QString& pSearchString) const
{
	QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const int rowCount = model->rowCount();

	int matchCount = 0;
	for (int sourceRow = 0; sourceRow < rowCount; ++sourceRow)
	{
		if (rowMatchesFilter(sourceRow, QModelIndex(), pCategories, pSearchString))
		{
			matchCount++;
		}
	}

	return matchCount;
}


bool ProviderCategoryFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const
{
	return rowMatchesFilter(pSourceRow, pSourceParent, mSelectedCategories, mSearchString);
}


bool ProviderCategoryFilterModel::rowMatchesFilter(int pSourceRow, const QModelIndex& pSourceParent, const QSet<QString>& pSelectedCategories, const QString& pSearchString) const
{
	QAbstractItemModel* const model = sourceModel();
	Q_ASSERT(model != nullptr);
	const QModelIndex idx = model->index(pSourceRow, 0, pSourceParent);

	bool isRowInSelectedCategory = pSelectedCategories.isEmpty() ||
			pSelectedCategories.contains(QLatin1String("all")) ||
			pSelectedCategories.contains(model->data(idx, ProviderModel::CATEGORY).toString().toLower());

	if (!isRowInSelectedCategory)
	{
		return false;
	}
	else if (pSearchString.isEmpty())
	{
		return true;
	}

	const QString display = model->data(idx, Qt::DisplayRole).toString();
	const QString shortname = model->data(idx, ProviderModel::SHORTNAME).toString();
	const QString longname = model->data(idx, ProviderModel::LONGNAME).toString();
	const QString address = model->data(idx, ProviderModel::ADDRESS).toString();
	const QString homepage = model->data(idx, ProviderModel::HOMEPAGE).toString();
	const QString shortdescription = model->data(idx, ProviderModel::SHORTDESCRIPTION).toString();
	const QString longdescription = model->data(idx, ProviderModel::LONGDESCRIPTION).toString();

	return display.contains(pSearchString, Qt::CaseInsensitive) ||
		   shortname.contains(pSearchString, Qt::CaseInsensitive) ||
		   longname.contains(pSearchString, Qt::CaseInsensitive) ||
		   address.contains(pSearchString, Qt::CaseInsensitive) ||
		   homepage.contains(pSearchString, Qt::CaseInsensitive) ||
		   shortdescription.contains(pSearchString, Qt::CaseInsensitive) ||
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
	bool filterChanged = false;
	const auto excludedCategories = ProviderModel::getProviderCategories() - mSelectedCategories;
	for (const QString& category : excludedCategories)
	{
		if (resultCountForFilter(QSet<QString>({category}), mSearchString) > 0)
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
