/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SortedReaderDriverModel.h"

#include "ReaderDriverModel.h"

using namespace governikus;

bool SortedReaderDriverModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{

	const auto& readerSupportedLeft = sourceModel()->data(source_left, ReaderDriverModel::READER_SUPPORTED).toBool();
	const auto& readerSupportedRight = sourceModel()->data(source_right, ReaderDriverModel::READER_SUPPORTED).toBool();

	const auto& readerInstalledLeft = sourceModel()->data(source_left, ReaderDriverModel::READER_INSTALLED).toBool();
	const auto& readerInstalledRight = sourceModel()->data(source_right, ReaderDriverModel::READER_INSTALLED).toBool();

	if (readerSupportedLeft == readerSupportedRight)
	{
		return readerInstalledLeft < readerInstalledRight;
	}
	return readerSupportedLeft < readerSupportedRight;
}


void SortedReaderDriverModel::onDataChanged()
{
	sort(0, Qt::DescendingOrder);
}
