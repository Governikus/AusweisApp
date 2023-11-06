/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSortFilterProxyModel>

class test_RemoteDeviceFilterModel;

namespace governikus
{

class RemoteDeviceFilterModel
	: public QSortFilterProxyModel
{
	Q_OBJECT

	friend class ::test_RemoteDeviceFilterModel;

	private:
		using FilterFunctionType = std::function<bool (int pSourceRow, const QModelIndex& pSourceParent)>;
		FilterFunctionType mFilterToApply;

		RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, const FilterFunctionType& pFilterFunction);

	public:
		struct ShowAvailableAndPaired {};
		struct ShowUnavailableAndPaired {};
		struct ShowActivePairingMode {};

		static ShowAvailableAndPaired showAvailableAndPaired;
		static ShowUnavailableAndPaired showUnavailableAndPaired;
		static ShowActivePairingMode showActivePairingMode;

		RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, ShowAvailableAndPaired);
		RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, ShowUnavailableAndPaired);
		RemoteDeviceFilterModel(QAbstractItemModel* pSourceModel, ShowActivePairingMode);

		~RemoteDeviceFilterModel() override = default;

	private:
		[[nodiscard]] bool available(int pSourceRow, const QModelIndex& pSourceParent) const;
		[[nodiscard]] bool isDevicePaired(int pSourceRow, const QModelIndex& pSourceParent) const;

		[[nodiscard]] bool availableNotPairing(int pSourceRow, const QModelIndex& pSourceParent) const;
		[[nodiscard]] bool unavailableAndPaired(int pSourceRow, const QModelIndex& pSourceParent) const;
		[[nodiscard]] bool isPairing(int pSourceRow, const QModelIndex& pSourceParent) const;

	protected:
		[[nodiscard]] bool filterAcceptsRow(int pSourceRow, const QModelIndex& pSourceParent) const override;
};

} // namespace governikus
