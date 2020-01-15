/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSortFilterProxyModel>

namespace governikus
{

class HistoryProxyModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	public:
		Q_INVOKABLE bool removeRows(int pRow, int pCount, const QModelIndex& pParent = QModelIndex()) override;

		HistoryProxyModel();

		virtual ~HistoryProxyModel() override;
};


} // namespace governikus
