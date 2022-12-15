/*!
 * \brief Unit tests for \ref RemoteReaderAdvertiserImpl
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReaderAdvertiser.h"

#include "DatagramHandler.h"
#include "Env.h"
#include "messages/Discovery.h"

#include <QNetworkProxy>
#include <QtTest>

using namespace governikus;


class DatagramHandlerMock
	: public DatagramHandler
{
	Q_OBJECT

	public:
		[[nodiscard]] bool isBound() const override;

		void send(const QByteArray& pData) override
		{
			Q_EMIT fireSend(pData);
		}

	Q_SIGNALS:
		void fireSend(const QByteArray& pData);
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


		void advertisePeriodically()
		{
			const QString ifdName("ServerName");
			const QString ifdId("0123456789ABCDEF");
			quint16 port = 12345;
			int pTimerInterval = 100;

			QSignalSpy spy(mMock.data(), &DatagramHandlerMock::fireSend);
			QScopedPointer<RemoteReaderAdvertiser> advertiser(Env::create<RemoteReaderAdvertiser*>(ifdName, ifdId, port, pTimerInterval));
			QTRY_COMPARE(spy.count(), 5); // clazy:exclude=qstring-allocations
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

			const QString ifdName("ServerName");
			const QString ifdId("0123456789ABCDEF");
			quint16 port = 12345;
			int pTimerInterval = 100;

			QSignalSpy spy(mMock.data(), &DatagramHandlerMock::fireSend);
			QScopedPointer<RemoteReaderAdvertiser> advertiser(Env::create<RemoteReaderAdvertiser*>(ifdName, ifdId, port, pTimerInterval));
			advertiser->setPairing(pairing);
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			advertiser.reset();

			const auto& byteArray = spy.at(0).at(0).toByteArray();
			const auto& offerMsg = Discovery(QJsonDocument::fromJson(byteArray).object());

			QCOMPARE(offerMsg.getIfdName(), ifdName);
			QCOMPARE(offerMsg.getIfdId(), ifdId);
			QCOMPARE(offerMsg.getPort(), port);
			QCOMPARE(offerMsg.getSupportedApis(), IfdVersion::supported());
			QCOMPARE(offerMsg.getPairing(), pairing);
		}


};

Q_DECLARE_METATYPE(QHostAddress)

QTEST_GUILESS_MAIN(test_RemoteReaderAdvertiser)
#include "test_RemoteReaderAdvertiser.moc"
