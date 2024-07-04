/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref FuncUtils
 */

#include "FuncUtils.h"

#include <QtTest>

using namespace governikus;

class Reader
{
	private:
		QString mName;
		bool mConnected;

	public:
		Reader();
		Reader(const QString& pName, bool pConnected);
		~Reader() = default;

		[[nodiscard]] const QString& getName() const;
		[[nodiscard]] bool isConnected() const;
};


Reader::Reader()
	: mName()
	, mConnected(false)
{
}


Reader::Reader(const QString& pName, bool pConnected)
	: mName(pName)
	, mConnected(pConnected)
{
}


const QString& Reader::getName() const
{
	return mName;
}


bool Reader::isConnected() const
{
	return mConnected;
}


class test_FuncUtils
	: public QObject
{
	Q_OBJECT

	private:
		QList<Reader> mReaders;

	private Q_SLOTS:
		void initTestCase()
		{
			mReaders = QList<Reader>({Reader(QStringLiteral("NFC Reader"), true),
									  Reader(QStringLiteral("Remote Reader"), false),
									  Reader(QStringLiteral("PCSC Reader"), true)});
		}


		void testMapWithNonVoid()
		{
			const QList<QString> readerNamesVector = map<Reader, QString>([](const Reader& r){
					return r.getName();
				}, mReaders);

			QCOMPARE(readerNamesVector.size(), 3);
			QCOMPARE(readerNamesVector.at(0), QStringLiteral("NFC Reader"));
			QCOMPARE(readerNamesVector.at(1), QStringLiteral("Remote Reader"));
			QCOMPARE(readerNamesVector.at(2), QStringLiteral("PCSC Reader"));

			const QList<QString> readerNamesList = map<Reader, QString>([](const Reader& r){
					return r.getName();
				}, mReaders);

			QCOMPARE(readerNamesList.size(), 3);
			QCOMPARE(readerNamesList.at(0), QStringLiteral("NFC Reader"));
			QCOMPARE(readerNamesList.at(1), QStringLiteral("Remote Reader"));
			QCOMPARE(readerNamesList.at(2), QStringLiteral("PCSC Reader"));
		}


		void testFilter()
		{
			const QList<Reader> connectedReaders = filter<Reader>([](const Reader& r){
					return r.isConnected();
				}, mReaders);

			QCOMPARE(connectedReaders.size(), 2);
			QCOMPARE(connectedReaders.at(0).getName(), QStringLiteral("NFC Reader"));
			QCOMPARE(connectedReaders.at(1).getName(), QStringLiteral("PCSC Reader"));
		}


};


QTEST_GUILESS_MAIN(test_FuncUtils)
#include "test_FuncUtils.moc"
