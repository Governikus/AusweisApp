/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

#include "CheckIDCardModel.h"

#include "ReaderInfo.h"

#include <QtTest>


using namespace governikus;


class test_CheckIDCardModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_onCardInserted()
		{
			CheckIDCardModel model;
			QSignalSpy spyResultChanged(&model, &CheckIDCardModel::fireResultChanged);
			QSignalSpy spyScanCompleted(&model, &CheckIDCardModel::fireScanCompleted);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN);

			ReaderInfo readerInfo(QStringLiteral("TestReader"));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 0);
			QCOMPARE(spyScanCompleted.count(), 0);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN);

			QTest::ignoreMessage(QtWarningMsg, "Detected multiple readers with cards");
			const ReaderInfo wrongReaderInfo(QStringLiteral("OtherName"));
			model.onCardInserted(wrongReaderInfo);
			QCOMPARE(spyResultChanged.count(), 0);
			QCOMPARE(spyScanCompleted.count(), 0);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN);

			readerInfo.setCardInfo(CardInfo(CardType::UNKNOWN));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 1);
			QCOMPARE(spyScanCompleted.count(), 1);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN_CARD_DETECTED);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 2);
			QCOMPARE(spyScanCompleted.count(), 2);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::SUCCESS);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD, FileRef::appEId(), nullptr, 0));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 3);
			QCOMPARE(spyScanCompleted.count(), 3);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::PIN_BLOCKED);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD, FileRef::appEId(), nullptr, 1));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 4);
			QCOMPARE(spyScanCompleted.count(), 4);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::PIN_SUSPENDED);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD, FileRef::appEId(), nullptr, 3, true));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 5);
			QCOMPARE(spyScanCompleted.count(), 5);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::PIN_DEACTIVATED);

			readerInfo.setMaxApduLength(20);
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 6);
			QCOMPARE(spyScanCompleted.count(), 6);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::INSUFFICIENT_APDU_LENGTH);
		}


};

QTEST_MAIN(test_CheckIDCardModel)
#include "test_CheckIDCardModel.moc"
