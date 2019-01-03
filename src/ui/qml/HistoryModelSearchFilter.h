/*!
 * \brief A filter to search the history model
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QPointer>
#include <QSortFilterProxyModel>

namespace governikus
{

class HistoryModelSearchFilter
	: public QSortFilterProxyModel
{
	Q_OBJECT

	private:
		QString mFilterString;

	protected:
		bool filterAcceptsRow(int pSourceRow, const QModelIndex&) const override;

	public:
		Q_INVOKABLE void setFilterString(const QString& pFilterString);
};

} // namespace governikus
