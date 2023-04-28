/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref PcscReader
 */

#include "PcscReader.h"

#include <QtTest>

using namespace governikus;


class test_PcscReader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void createDestroy()
		{
#if !(defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) && !defined(Q_OS_FREEBSD)
			QSKIP("Using LD_PRELOAD is not supported");
#endif

			const QString readerName = QStringLiteral("PCSC");

			QTest::ignoreMessage(QtDebugMsg, QStringLiteral("\"%1\"").arg(readerName).toUtf8().data());
			QScopedPointer<PcscReader> reader(new PcscReader(readerName));
			QCOMPARE(reader->objectName(), readerName);
			QVERIFY(reader->getState().szReader);
			QVERIFY(!reader->hasFeature(FeatureID::TLV_PROPERTIES));

			QTest::ignoreMessage(QtDebugMsg, QStringLiteral("\"%1\"").arg(readerName).toUtf8().data());
			QTest::ignoreMessage(QtDebugMsg, QStringLiteral("\"%1\"").arg(readerName).toUtf8().data());
			reader.reset(new PcscReader(readerName));

			QCOMPARE(reader->init(), PcscUtils::Scard_S_Success);
			QVERIFY(reader->hasFeature(FeatureID::TLV_PROPERTIES));
			QCOMPARE(reader->getFeatureValue(FeatureID::TLV_PROPERTIES), 1110638610);

			QTest::ignoreMessage(QtDebugMsg, QStringLiteral("\"%1\"").arg(readerName).toUtf8().data());
			reader.reset();
		}


};

QTEST_GUILESS_MAIN(test_PcscReader)
#include "test_PcscReader.moc"
