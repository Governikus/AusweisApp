/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtTest>

#include "ReaderDriverModel.h"

using namespace governikus;

struct FakeReaderInfo
{
	QString readerName;
	bool readerInstalled;
	bool readerSupported;
};


class MockReaderDriverModel
	: public QAbstractTableModel
{
	Q_OBJECT

	private:
		QVector<FakeReaderInfo> mReaders;

	public:
		explicit MockReaderDriverModel(const QVector<FakeReaderInfo>& readers);


		int rowCount(const QModelIndex& parent) const override;


		int columnCount(const QModelIndex& parent) const override;


		QVariant data(const QModelIndex& index, int role) const override;
};

MockReaderDriverModel::MockReaderDriverModel(const QVector<FakeReaderInfo>& readers)
	: mReaders(readers)
{
}


int MockReaderDriverModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return mReaders.count();
}


int MockReaderDriverModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 2;
}


QVariant MockReaderDriverModel::data(const QModelIndex& index, int role) const
{

	int readerIndex = index.row();

	ReaderDriverModel::UserRoles user_role = static_cast<ReaderDriverModel::UserRoles>(role);
	switch (user_role)
	{
		case governikus::ReaderDriverModel::READER_NAME:
			return mReaders[readerIndex].readerName;

		case governikus::ReaderDriverModel::READER_INSTALLED:
			return mReaders[readerIndex].readerInstalled;

		case governikus::ReaderDriverModel::READER_SUPPORTED:
			return mReaders[readerIndex].readerSupported;

		default:
			Q_ASSERT(false); // MockReaderDriverModel does not implement this role
			return QVariant();
	}
}


class test_SortedReaderDriverModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_order()
		{
			QVector<FakeReaderInfo> readers = {
				{"3", false, false},
				{"0", true, true},
				{"1", false, true},
				{"2", true, false}
			};
			MockReaderDriverModel sourceModel(readers);
			SortedReaderDriverModel sortedModel;

			sortedModel.setSourceModel(&sourceModel);
			sortedModel.onDataChanged();

			QVERIFY(sortedModel.rowCount() == 4);
			QCOMPARE(sortedModel.data(sortedModel.index(0, ReaderDriverModel::ColumnId::ReaderName, QModelIndex()), ReaderDriverModel::UserRoles::READER_NAME), "0");
			QCOMPARE(sortedModel.data(sortedModel.index(1, ReaderDriverModel::ColumnId::ReaderName, QModelIndex()), ReaderDriverModel::UserRoles::READER_NAME), "1");
			QCOMPARE(sortedModel.data(sortedModel.index(2, ReaderDriverModel::ColumnId::ReaderName, QModelIndex()), ReaderDriverModel::UserRoles::READER_NAME), "2");
			QCOMPARE(sortedModel.data(sortedModel.index(3, ReaderDriverModel::ColumnId::ReaderName, QModelIndex()), ReaderDriverModel::UserRoles::READER_NAME), "3");
		}


};

QTEST_GUILESS_MAIN(test_SortedReaderDriverModel)
#include "test_SortedReaderDriverModel.moc"
