/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilterModel.h"
#include "LogModel.h"

using namespace governikus;

void LogFilterModel::onLevelsChanged()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	beginFilterChange();
#endif

	mSelectedLevels.intersect(mSourceModel->getLevels());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	endFilterChange();
#else
	invalidateFilter();
#endif
	Q_EMIT fireLevelsChanged();
}


void LogFilterModel::onCategoriesChanged()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	beginFilterChange();
#endif

	mSelectedCategories.intersect(mSourceModel->getCategories());

#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	endFilterChange();
#else
	invalidateFilter();
#endif
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
	, mSourceModel(nullptr)
{

}


QStringList LogFilterModel::getLevels() const
{
	if (!mSourceModel)
	{
		return {};
	}

	const auto& level = mSourceModel->getLevels();
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
	if (!mSourceModel)
	{
		return {};
	}

	const auto& categories = mSourceModel->getCategories();
	QStringList list(categories.constBegin(), categories.constEnd());
	list.sort(Qt::CaseInsensitive);
	return list;
}


QStringList LogFilterModel::getSelectedCategories() const
{
	return QStringList(mSelectedCategories.constBegin(), mSelectedCategories.constEnd());
}


void LogFilterModel::setSourceModel(QAbstractItemModel* pSourceModel)
{
	auto* logModel = qobject_cast<LogModel*>(pSourceModel);
	if (logModel == nullptr)
	{
		qWarning() << "Source model isn't a LogModel";
		return;
	}

	if (mSourceModel == logModel)
	{
		return;
	}

	if (mSourceModel)
	{
		disconnect(mSourceModel, &LogModel::fireLevelsChanged, this, &LogFilterModel::onLevelsChanged);
		disconnect(mSourceModel, &LogModel::fireCategoriesChanged, this, &LogFilterModel::onCategoriesChanged);
	}

	mSourceModel = logModel;
	QSortFilterProxyModel::setSourceModel(mSourceModel);

	connect(mSourceModel, &LogModel::fireLevelsChanged, this, &LogFilterModel::onLevelsChanged);
	connect(mSourceModel, &LogModel::fireCategoriesChanged, this, &LogFilterModel::onCategoriesChanged);

	onLevelsChanged();
	onCategoriesChanged();
}


void LogFilterModel::configureLevel(const QString& pLevel, bool pEnabled)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	beginFilterChange();
#endif

	if (pEnabled)
	{
		mSelectedLevels.insert(pLevel);
	}
	else
	{
		mSelectedLevels.remove(pLevel);
	}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	endFilterChange();
#else
	invalidateFilter();
#endif

	Q_EMIT fireLevelsChanged();
}


void LogFilterModel::configureCategory(const QString& pCategory, bool pEnabled)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	beginFilterChange();
#endif
	if (pEnabled)
	{
		mSelectedCategories.insert(pCategory);
	}
	else
	{
		mSelectedCategories.remove(pCategory);
	}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	endFilterChange();
#else
	invalidateFilter();
#endif

	Q_EMIT fireCategoriesChanged();
}
