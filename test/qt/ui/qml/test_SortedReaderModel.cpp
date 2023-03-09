/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include <QtTest>

#include "ReaderModel.h"

using namespace governikus;

struct FakeReaderInfo
{
	QString readerName;
	bool readerInstalled;
	bool readerSupported;
};


class MockReaderModel
	: public QAbstractTableModel
{
	Q_OBJECT

	private:
		QVector<FakeReaderInfo> mReaders;

	public:
		explicit MockReaderModel(const QVector<FakeReaderInfo>& readers);


		int rowCount(const QModelIndex& parent) const override;


		int columnCount(const QModelIndex& parent) const override;


		QVariant data(const QModelIndex& index, int role) const override;
};

MockReaderModel::MockReaderModel(const QVector<FakeReaderInfo>& readers)
	: mReaders(readers)
{
}


int MockReaderModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return mReaders.count();
}


int MockReaderModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 2;
}


QVariant MockReaderModel::data(const QModelIndex& index, int role) const
{

	int readerIndex = index.row();

	ReaderModel::UserRoles user_role = static_cast<ReaderModel::UserRoles>(role);
	switch (user_role)
	{
		case governikus::ReaderModel::READER_NAME:
			return mReaders[readerIndex].readerName;

		case governikus::ReaderModel::READER_INSTALLED:
			return mReaders[readerIndex].readerInstalled;

		case governikus::ReaderModel::READER_SUPPORTED:
			return mReaders[readerIndex].readerSupported;

		default:
			Q_ASSERT(false); // MockReaderModel does not implement this role
			return QVariant();
	}
}


class test_SortedReaderModel
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
			MockReaderModel sourceModel(readers);
			SortedReaderModel sortedModel;

			sortedModel.setSourceModel(&sourceModel);
			sortedModel.onDataChanged();

			QVERIFY(sortedModel.rowCount() == 4);
			QCOMPARE(sortedModel.data(sortedModel.index(0, 0, QModelIndex()), ReaderModel::UserRoles::READER_NAME), "0");
			QCOMPARE(sortedModel.data(sortedModel.index(1, 0, QModelIndex()), ReaderModel::UserRoles::READER_NAME), "1");
			QCOMPARE(sortedModel.data(sortedModel.index(2, 0, QModelIndex()), ReaderModel::UserRoles::READER_NAME), "2");
			QCOMPARE(sortedModel.data(sortedModel.index(3, 0, QModelIndex()), ReaderModel::UserRoles::READER_NAME), "3");
		}


};

QTEST_GUILESS_MAIN(test_SortedReaderModel)
#include "test_SortedReaderModel.moc"
