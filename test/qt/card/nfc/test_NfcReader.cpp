/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "NfcReader.h"

#include <QtTest>


using namespace governikus;


class test_NfcReader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
#if !defined(LIBS_GOVERNIKUS) && QT_VERSION < QT_VERSION_CHECK(6, 11, 2)
			QSKIP("https://codereview.qt-project.org/c/qt/qtconnectivity/+/733785");
#endif
		}


		void adapterState()
		{
			NfcReader reader;
			QSignalSpy spy(&reader, &NfcReader::fireNfcAdapterStateChanged);

			reader.adapterStateChanged(QNearFieldManager::AdapterState::Offline);
			QCOMPARE(spy.size(), 1);
			QCOMPARE(spy.at(0).at(0), false);

			reader.adapterStateChanged(QNearFieldManager::AdapterState::TurningOn);
			QCOMPARE(spy.size(), 2);
			QCOMPARE(spy.at(1).at(0), false);

			reader.adapterStateChanged(QNearFieldManager::AdapterState::Online);
			QCOMPARE(spy.size(), 3);
			QCOMPARE(spy.at(2).at(0), true);

			reader.adapterStateChanged(QNearFieldManager::AdapterState::TurningOff);
			QCOMPARE(spy.size(), 4);
			QCOMPARE(spy.at(3).at(0), false);
		}


		void targetDetected()
		{
			auto* target1 = new QNearFieldTarget();
			auto* target2 = new QNearFieldTarget();
			NfcReader reader;

			reader.targetDetected(nullptr);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetDetected: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, "The target does not provide commands");
			reader.targetDetected(target1);

			QTest::ignoreMessage(QtDebugMsg, "Card created");
			reader.mCard.reset(new NfcCard(target1));
			QVERIFY(reader.getCard());
			QCOMPARE(reader.mTargetQueue.size(), 0);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetDetected: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("Card already inserted, enqueuing new QNearFieldTarget\\(.*\\)")));
			reader.targetDetected(target2);
			QCOMPARE(reader.mTargetQueue.size(), 1);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetDetected: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("Card already inserted, enqueuing new QNearFieldTarget\\(.*\\)")));
			reader.targetDetected(target1);
			QCOMPARE(reader.mTargetQueue.size(), 2);

			reader.mCard.reset();
			delete target2;
		}


		void targetLost()
		{
			auto* target1 = new QNearFieldTarget();
			auto* target2 = new QNearFieldTarget();
			NfcReader reader;
			QSignalSpy spy(&reader, &NfcReader::fireCardRemoved);

			reader.targetLost(nullptr);
			QCOMPARE(spy.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			reader.targetLost(target1);
			QCOMPARE(spy.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Card created");
			reader.mCard.reset(new NfcCard(target1));
			QVERIFY(reader.getCard());
			QCOMPARE(spy.count(), 0);
			QCOMPARE(reader.mTargetQueue.size(), 0);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			reader.targetLost(target2);
			QVERIFY(reader.getCard());
			QCOMPARE(spy.count(), 0);
			QCOMPARE(reader.mTargetQueue.size(), 0);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			reader.targetLost(target1);
			QVERIFY(!reader.getCard());
			QCOMPARE(spy.count(), 0);
			QCOMPARE(reader.mTargetQueue.size(), 0);

			QTest::ignoreMessage(QtDebugMsg, "Card created");
			reader.mCard.reset(new NfcCard(target2));
			reader.setInfoCardInfo(CardInfo(CardType::EID_CARD));
			QVERIFY(reader.getCard());
			QCOMPARE(spy.count(), 0);
			QCOMPARE(reader.mTargetQueue.size(), 0);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtInfoMsg, "Card removed");
			reader.targetLost(target2);
			QVERIFY(!reader.getCard());
			QCOMPARE(spy.count(), 1);
			QCOMPARE(reader.mTargetQueue.size(), 0);
		}


		void queue()
		{
			auto* target1 = new QNearFieldTarget();
			auto* target2 = new QNearFieldTarget();
			auto* target3 = new QNearFieldTarget();
			NfcReader reader;

			QTest::ignoreMessage(QtDebugMsg, "Card created");
			reader.mCard.reset(new NfcCard(target1));
			QVERIFY(reader.getCard());

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetDetected: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("Card already inserted, enqueuing new QNearFieldTarget\\(.*\\)")));
			reader.targetDetected(target2);
			QVERIFY(reader.getCard());
			QCOMPARE(reader.mTargetQueue.size(), 1);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			reader.targetLost(target3);
			QVERIFY(reader.getCard());
			QCOMPARE(reader.mTargetQueue.size(), 1);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			reader.targetLost(target2);
			QVERIFY(reader.getCard());
			QCOMPARE(reader.mTargetQueue.size(), 0);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetDetected: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("Card already inserted, enqueuing new QNearFieldTarget\\(.*\\)")));
			reader.targetDetected(target2);
			QVERIFY(reader.getCard());
			QCOMPARE(reader.mTargetQueue.size(), 1);

			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetLost: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("Using enqueued QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("targetDetected: QNearFieldTarget\\(.*\\)")));
			QTest::ignoreMessage(QtDebugMsg, "The target does not provide commands");
			reader.targetLost(target1);
			QVERIFY(!reader.getCard());
			QCOMPARE(reader.mTargetQueue.size(), 0);

			delete target2;
			delete target3;
		}


		void getCard()
		{
			NfcReader reader;
			QVERIFY(!reader.getCard());

			QTest::ignoreMessage(QtDebugMsg, "Card created");
			reader.mCard.reset(new NfcCard(nullptr));
			QVERIFY(reader.getCard());

			QVERIFY(reader.mCard->invalidateTarget(nullptr));
			QVERIFY(reader.getCard());
		}


};

QTEST_GUILESS_MAIN(test_NfcReader)
#include "test_NfcReader.moc"
