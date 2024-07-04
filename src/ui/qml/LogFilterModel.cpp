/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilterModel.h"

#include "LogModel.h"


using namespace governikus;


void LogFilterModel::onLevelsChanged()
{
	mSelectedLevels.intersect(Env::getSingleton<LogModel>()->getLevels());
	invalidateFilter();
	Q_EMIT fireLevelsChanged();
}


void LogFilterModel::onCategoriesChanged()
{
	mSelectedCategories.intersect(Env::getSingleton<LogModel>()->getCategories());
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


QStringList LogFilterModel::getLevels() const
{
	const auto& level = Env::getSingleton<LogModel>()->getLevels();
	QStringList list(level.constBegin(), level.constEnd());
	list.sort(Qt::CaseInsensitive);
	return list;
}


QStringList LogFilterModel::getSelectedLevels() const
{
	return QStringList(mSelectedLevels.constBegin(), mSelectedLevels.constEnd());
}


QStringList LogFilterModel::getCategories() const
{
	const auto& categories = Env::getSingleton<LogModel>()->getCategories();
	QStringList list(categories.constBegin(), categories.constEnd());
	list.sort(Qt::CaseInsensitive);
	return list;
}


QStringList LogFilterModel::getSelectedCategories() const
{
	return QStringList(mSelectedCategories.constBegin(), mSelectedCategories.constEnd());
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
