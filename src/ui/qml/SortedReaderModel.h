/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

namespace governikus
{

class SortedReaderModel
	: public QSortFilterProxyModel
{
	Q_OBJECT
	QML_UNCREATABLE("Used by ReaderModel only")
	QML_ELEMENT

	protected:
		[[nodiscard]] bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

	public Q_SLOTS:
		void onDataChanged();


};

} // namespace governikus
