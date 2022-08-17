/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSortFilterProxyModel>

namespace governikus
{

class SortedReaderModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	protected:
		bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

	public Q_SLOTS:
		void onDataChanged();


};

} // namespace governikus
