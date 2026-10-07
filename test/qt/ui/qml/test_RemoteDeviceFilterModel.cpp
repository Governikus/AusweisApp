/**
 * Copyright (c) 2023-2026 Governikus Service GmbH, Germany
 */

#include "RemoteDeviceFilterModel.h"

#include "RemoteDeviceModel.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_RemoteDeviceFilterModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_FilterAcceptsRow()
		{
			QSharedPointer<RemoteDeviceModel> sourceModel = QSharedPointer<RemoteDeviceModel>::create();
			QSharedPointer<RemoteDeviceFilterModel> filterModelAvail = QSharedPointer<RemoteDeviceFilterModel>::create(sourceModel.data(), RemoteDeviceFilterModel::showAvailableAndPaired);
			QSharedPointer<RemoteDeviceFilterModel> filterModelUnavail = QSharedPointer<RemoteDeviceFilterModel>::create(sourceModel.data(), RemoteDeviceFilterModel::showUnavailableAndPaired);
			QSharedPointer<RemoteDeviceFilterModel> filterModelAvailPair = QSharedPointer<RemoteDeviceFilterModel>::create(sourceModel.data(), RemoteDeviceFilterModel::showActivePairingMode);

			RemoteServiceSettings::RemoteInfo info1("test id"_ba, QDateTime(QDate(2019, 5, 14), QTime(0, 0)), "reader 1"_L1);
			const RemoteDeviceModelEntry availableEntry(info1, true, false, true, false, nullptr);
			RemoteServiceSettings::RemoteInfo info2("test id"_ba, QDateTime(QDate(2019, 5, 14), QTime(0, 0)), "reader 2"_L1);
			const RemoteDeviceModelEntry unavailableEntry(info2, false, false, true, false, nullptr);
			const RemoteDeviceModelEntry availablePairingEntry(info2, true, false, true, true, nullptr);

			sourceModel->mAllRemoteReaders.insert(0, availableEntry);
			sourceModel->mAllRemoteReaders.insert(1, unavailableEntry);
			sourceModel->mAllRemoteReaders.insert(2, availablePairingEntry);

			QCOMPARE(filterModelAvail->filterAcceptsRow(0, QModelIndex()), true);
			QCOMPARE(filterModelAvail->filterAcceptsRow(1, QModelIndex()), false);

			QCOMPARE(filterModelUnavail->filterAcceptsRow(0, QModelIndex()), false);
			QCOMPARE(filterModelUnavail->filterAcceptsRow(1, QModelIndex()), true);

			QCOMPARE(filterModelAvailPair->filterAcceptsRow(0, QModelIndex()), false);
			QCOMPARE(filterModelAvailPair->filterAcceptsRow(1, QModelIndex()), false);
			QCOMPARE(filterModelAvailPair->filterAcceptsRow(2, QModelIndex()), true);
		}


};

QTEST_MAIN(test_RemoteDeviceFilterModel)
#include "test_RemoteDeviceFilterModel.moc"
