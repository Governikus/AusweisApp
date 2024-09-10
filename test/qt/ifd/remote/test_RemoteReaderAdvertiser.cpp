/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref RemoteReaderAdvertiserImpl
 */

#include "RemoteReaderAdvertiser.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "messages/Discovery.h"

#include <QNetworkProxy>
#include <QtTest>

namespace governikus
{


template<> RemoteReaderAdvertiser* createNewObject<RemoteReaderAdvertiser*, const QString&, const QString&, quint16&, int&, bool&>(const QString& pIfdName, const QString& pIfdId, quint16& pPort, int& pInterval, bool& pPairing)
{
	return new RemoteReaderAdvertiserImpl(pIfdName, pIfdId, pPort, pPairing, pInterval);
}


} // namespace governikus


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class DatagramHandlerMock
	: public DatagramHandler
{
	Q_OBJECT

	public:
		QByteArrayList mList;

		[[nodiscard]] bool isBound() const override;

		void send(const QByteArray& pData) override
		{
			mList << pData;
		}


};


bool DatagramHandlerMock::isBound() const
{
	return true;
}


class test_RemoteReaderAdvertiser
	: public QObject
{
	Q_OBJECT

	QPointer<DatagramHandlerMock> mMock;

	private Q_SLOTS:
		void init()
		{
			mMock = new DatagramHandlerMock();
			std::function<DatagramHandler* (bool pListen)> creator = [this](bool){
						return mMock.data();
					};
			Env::setCreator<DatagramHandler*>(creator);
		}


		void cleanup()
		{
			QVERIFY(mMock == nullptr);
		}


		void checkBroadcast_data()
		{
			QTest::addColumn<bool>("pairing");
			QTest::newRow("Pairing enabled") << true;
			QTest::newRow("Pairing disabled") << false;
		}


		void checkBroadcast()
		{
			QFETCH(bool, pairing);

			const QString ifdName("ServerName"_L1);
			const QString ifdId("0123456789ABCDEF"_L1);
			quint16 port = 12345;
			int pTimerInterval = 99999;

			QScopedPointer<RemoteReaderAdvertiser> advertiser(Env::create<RemoteReaderAdvertiser*>(ifdName, ifdId, port, pTimerInterval, pairing));
			QTRY_COMPARE(mMock->mList.size(), 1); // clazy:exclude=qstring-allocations

			const auto data = mMock->mList.at(0);
			advertiser.reset();
			const auto& offerMsg = Discovery(QJsonDocument::fromJson(data).object());

			QCOMPARE(offerMsg.getIfdName(), ifdName);
			QCOMPARE(offerMsg.getIfdId(), ifdId.toLower());
			QCOMPARE(offerMsg.getPort(), port);
			QCOMPARE(offerMsg.getSupportedApis(), IfdVersion::supported());
			QCOMPARE(offerMsg.getPairing(), pairing);
		}


};

Q_DECLARE_METATYPE(QHostAddress)

QTEST_GUILESS_MAIN(test_RemoteReaderAdvertiser)
#include "test_RemoteReaderAdvertiser.moc"
