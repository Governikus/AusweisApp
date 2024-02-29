/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteDeviceFilterModel.h"

#include "RemoteDeviceModel.h"

using namespace governikus;
using namespace std::placeholders;


RemoteDeviceFilterModel::RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, const FilterFunctionType& pFilterFunction)
	: QSortFilterProxyModel()
	, mFilterToApply(pFilterFunction)
{
	QSortFilterProxyModel::setSourceModel(pSourceModel);
}


RemoteDeviceFilterModel::ShowAvailableAndPaired RemoteDeviceFilterModel::showAvailableAndPaired = {};
RemoteDeviceFilterModel::ShowUnavailableAndPaired RemoteDeviceFilterModel::showUnavailableAndPaired = {};
RemoteDeviceFilterModel::ShowActivePairingMode RemoteDeviceFilterModel::showActivePairingMode = {};


RemoteDeviceFilterModel::RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, ShowAvailableAndPaired)
	: RemoteDeviceFilterModel(pSourceModel, std::bind(&RemoteDeviceFilterModel::availableNotPairing, this, _1, _2))
{
}


RemoteDeviceFilterModel::RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, ShowUnavailableAndPaired)
	: RemoteDeviceFilterModel(pSourceModel, std::bind(&RemoteDeviceFilterModel::unavailableAndPaired, this, _1, _2))
{
}


RemoteDeviceFilterModel::RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, ShowActivePairingMode)
	: RemoteDeviceFilterModel(pSourceModel, std::bind(&RemoteDeviceFilterModel::isPairing, this, _1, _2))
{
}


bool RemoteDeviceFilterModel::available(int pSourceRow, const QModelIndex& pSourceParent) const
{
	const QModelIndex index = sourceModel()->index(pSourceRow, 0, pSourceParent);
	const bool isNetworkVisible = sourceModel()->data(index, RemoteDeviceModel::SettingsRemoteRoles::IS_NETWORK_VISIBLE).toBool();
	return isNetworkVisible;
}


bool RemoteDeviceFilterModel::isDevicePaired(int pSourceRow, const QModelIndex& pSourceParent) const
{
	const QModelIndex index = sourceModel()->index(pSourceRow, 0, pSourceParent);
	return sourceModel()->data(index, RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRED).toBool();
}


bool RemoteDeviceFilterModel::availableNotPairing(int pSourceRow, const QModelIndex& pSourceParent) const
{
	return available(pSourceRow, pSourceParent) && !isPairing(pSourceRow, pSourceParent);
}


bool RemoteDeviceFilterModel::unavailableAndPaired(int pSourceRow, const QModelIndex& pSourceParent) const
{
	return !available(pSourceRow, pSourceParent) && isDevicePaired(pSourceRow, pSourceParent);
}


bool RemoteDeviceFilterModel::isPairing(int pSourceRow, const QModelIndex& pSourceParent) const
{
	const QModelIndex index = sourceModel()->index(pSourceRow, 0, pSourceParent);
	return sourceModel()->data(index, RemoteDeviceModel::SettingsRemoteRoles::IS_PAIRING).toBool();
}


bool RemoteDeviceFilterModel::filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const
{
	return mFilterToApply(pSourceRow, pSourceParent);
}
