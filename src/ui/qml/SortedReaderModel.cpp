/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SortedReaderModel.h"

#include "ReaderModel.h"

using namespace governikus;

bool SortedReaderModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
	const auto& readerSupportedLeft = sourceModel()->data(source_left, ReaderModel::READER_SUPPORTED).toBool();
	const auto& readerSupportedRight = sourceModel()->data(source_right, ReaderModel::READER_SUPPORTED).toBool();

	if (readerSupportedLeft == readerSupportedRight)
	{
		const auto& readerInstalledLeft = sourceModel()->data(source_left, ReaderModel::READER_INSTALLED).toBool();
		const auto& readerInstalledRight = sourceModel()->data(source_right, ReaderModel::READER_INSTALLED).toBool();

		return !readerInstalledLeft && readerInstalledRight;
	}
	return !readerSupportedLeft && readerSupportedRight;
}


void SortedReaderModel::onDataChanged()
{
	sort(0, Qt::DescendingOrder);
}
