/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryProxyModel.h"

using namespace governikus;


HistoryProxyModel::HistoryProxyModel()
{
}


HistoryProxyModel::~HistoryProxyModel()
{
}


bool HistoryProxyModel::removeRows(int pRow, int pCount, const QModelIndex& pParent)
{
	return QSortFilterProxyModel::removeRows(pRow, pCount, pParent);
}
