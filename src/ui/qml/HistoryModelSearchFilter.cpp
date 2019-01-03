/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryModelSearchFilter.h"

#include "HistoryModel.h"

#include <QDebug>

using namespace governikus;


bool HistoryModelSearchFilter::filterAcceptsRow(int pSourceRow, const QModelIndex&) const
{
	if (mFilterString.isEmpty())
	{
		return true;
	}

	const HistoryModel* const dataSourceModel = qobject_cast<HistoryModel*>(sourceModel());
	if (dataSourceModel == nullptr)
	{
		return false;
	}

	const QModelIndex& modelIndex = dataSourceModel->index(pSourceRow, 0);
	if (dataSourceModel->data(modelIndex, HistoryModel::DATETIME).toDateTime().toString(tr("dd.MM.yyyy")).contains(mFilterString, Qt::CaseInsensitive)
			|| dataSourceModel->data(modelIndex, HistoryModel::SUBJECT).toString().contains(mFilterString, Qt::CaseInsensitive)
			|| dataSourceModel->data(modelIndex, HistoryModel::PURPOSE).toString().contains(mFilterString, Qt::CaseInsensitive)
			|| dataSourceModel->data(modelIndex, HistoryModel::REQUESTEDDATA).toString().contains(mFilterString, Qt::CaseInsensitive))
	{
		return true;
	}

	return false;
}


void governikus::HistoryModelSearchFilter::setFilterString(const QString& pFilterString)
{
	mFilterString = pFilterString;
	invalidateFilter();
}
