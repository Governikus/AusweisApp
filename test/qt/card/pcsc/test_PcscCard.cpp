/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "PcscCard.h"

#include <QTest>


using namespace governikus;


class test_PcscCard
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_EstablishPaceChannel()
		{
#if !(defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) && !defined(Q_OS_FREEBSD)
			QSKIP("Using LD_PRELOAD is not supported");
#endif
			PcscReader reader(QStringLiteral("reader"));
			reader.mReaderFeatures = PcscReaderFeature(QByteArray::fromHex("200442000dcc"));
			reader.setInfoCardInfo(CardInfo(CardType::EID_CARD, FileRef(), nullptr, 3));

			PcscCard card(&reader);
			card.mCardHandle = 8;

			const auto& output = card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, QByteArray(), QByteArray());
			QCOMPARE(output.getReturnCode(), CardReturnCode::OK);
			QVERIFY(output.wrongPasswordUsed());
			QCOMPARE(output.getStatusCodeMseSetAt(), StatusCode::SUCCESS);
			QCOMPARE(output.getPaceResult(), PaceResult::INVALID_PIN_1);
		}


};

QTEST_GUILESS_MAIN(test_PcscCard)
#include "test_PcscCard.moc"
