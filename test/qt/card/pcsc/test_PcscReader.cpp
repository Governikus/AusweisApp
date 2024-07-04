/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref PcscReader
 */

#include "PcscReader.h"
#include "PcscUtils.h"

#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) || defined(Q_OS_FREEBSD)
	#include "pcscmock.h"
#endif

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

			QCOMPARE(reader->init(), pcsc::Scard_S_Success);
			QVERIFY(reader->hasFeature(FeatureID::TLV_PROPERTIES));
			QCOMPARE(reader->getFeatureValue(FeatureID::TLV_PROPERTIES), 1110638610);

			QTest::ignoreMessage(QtDebugMsg, QStringLiteral("\"%1\"").arg(readerName).toUtf8().data());
			reader.reset();
		}


		void test_updateCard_data()
		{
			QTest::addColumn<LONG>("mockResult");
			QTest::addColumn<QStringList>("expectedWarnings");

			QTest::addRow("Success") << static_cast<LONG>(SCARD_S_SUCCESS) << QStringList();
			QTest::addRow("Timeout") << static_cast<LONG>(SCARD_E_TIMEOUT) << QStringList();
			QTest::addRow("Unknown Reader") << static_cast<LONG>(SCARD_E_UNKNOWN_READER) << QStringList({
						QStringLiteral("SCardGetStatusChange: \"Scard_E_Unknown_Reader\""),
						QStringLiteral("Reader unknown, stop updating reader information")
					});
			QTest::addRow("Other") << static_cast<LONG>(SCARD_E_CANCELLED) << QStringList({
						QStringLiteral("SCardGetStatusChange: \"Scard_E_Cancelled\""),
						QStringLiteral("Cannot update reader")
					});
		}


		void test_updateCard()
		{
#if !(defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) && !defined(Q_OS_FREEBSD)
			QSKIP("Using LD_PRELOAD is not supported");
#endif
			QFETCH(LONG, mockResult);
			QFETCH(QStringList, expectedWarnings);

#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) || defined(Q_OS_FREEBSD)
			setResultGetCardStatus(mockResult);
#else
			Q_UNUSED(mockResult)
#endif

			QScopedPointer<PcscReader> reader(new PcscReader(QStringLiteral("PCSC")));

			for (const auto& message : expectedWarnings)
			{
				QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
			}

			if (expectedWarnings.isEmpty())
			{
				QTest::failOnWarning(QRegularExpression(QStringLiteral(".+")));
			}

			QCOMPARE(reader->init(), pcsc::Scard_S_Success);
		}


};

QTEST_GUILESS_MAIN(test_PcscReader)
#include "test_PcscReader.moc"
