/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilterModel.h"

#include "LogModel.h"


using namespace governikus;


void LogFilterModel::onLevelsChanged()
{
	mSelectedLevels.intersect(getLevels());
	invalidateFilter();
	Q_EMIT fireLevelsChanged();
}


void LogFilterModel::onCategoriesChanged()
{
	mSelectedCategories.intersect(getCategories());
	invalidateFilter();
	Q_EMIT fireCategoriesChanged();
}


bool LogFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const
{
	bool levelAccepted = true;
	bool categoryAccepted = true;

	if (!mSelectedLevels.isEmpty())
	{
		QModelIndex index = sourceModel()->index(pSourceRow, 0, pSourceParent);
		QString level = sourceModel()->data(index, LogModel::LogModelRoles::LevelRole).toString();
		levelAccepted = mSelectedLevels.contains(level);
	}

	if (!mSelectedCategories.isEmpty())
	{
		QModelIndex index = sourceModel()->index(pSourceRow, 0, pSourceParent);
		QString category = sourceModel()->data(index, LogModel::LogModelRoles::CategoryRole).toString();
		categoryAccepted = mSelectedCategories.contains(category);
	}

	return levelAccepted && categoryAccepted;
}


LogFilterModel::LogFilterModel()
	: QSortFilterProxyModel()
	, mSelectedLevels()
	, mSelectedCategories()
{
	const auto& logModel = Env::getSingleton<LogModel>();
	connect(logModel, &LogModel::fireLevelsChanged, this, &LogFilterModel::onLevelsChanged);
	connect(logModel, &LogModel::fireCategoriesChanged, this, &LogFilterModel::onCategoriesChanged);
	QSortFilterProxyModel::setSourceModel(logModel);
}


const QSet<QString>& LogFilterModel::getLevels() const
{
	return Env::getSingleton<LogModel>()->getLevels();
}


const QSet<QString>& LogFilterModel::getSelectedLevels() const
{
	return mSelectedLevels;
}


const QSet<QString>& LogFilterModel::getCategories() const
{
	return Env::getSingleton<LogModel>()->getCategories();
}


const QSet<QString>& LogFilterModel::getSelectedCategories() const
{
	return mSelectedCategories;
}


void LogFilterModel::configureLevel(const QString& pLevel, bool pEnabled)
{
	if (pEnabled)
	{
		mSelectedLevels.insert(pLevel);
	}
	else
	{
		mSelectedLevels.remove(pLevel);
	}

	invalidateFilter();
	Q_EMIT fireLevelsChanged();
}


void LogFilterModel::configureCategory(const QString& pCategory, bool pEnabled)
{
	if (pEnabled)
	{
		mSelectedCategories.insert(pCategory);
	}
	else
	{
		mSelectedCategories.remove(pCategory);
	}

	invalidateFilter();
	Q_EMIT fireCategoriesChanged();
}
