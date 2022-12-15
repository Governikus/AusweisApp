/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
		[[nodiscard]] Q_INVOKABLE bool removeRows(int pRow, int pCount, const QModelIndex& pParent = QModelIndex()) override;

		HistoryProxyModel() = default;

		~HistoryProxyModel() override = default;
};


} // namespace governikus
