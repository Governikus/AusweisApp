/*!
 * \brief Unit tests for \ref PcscUtils
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "PcscUtils.h"

#include <QtTest>

using namespace governikus;


class test_PcscUtils
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void checkNames_data()
		{
			QTest::addColumn<PCSC_RETURNCODE>("code");

			QTest::newRow("Scard_S_Success") << PCSC_RETURNCODE(SCARD_S_SUCCESS);
			QTest::newRow("Scard_E_Not_Transacted") << PCSC_RETURNCODE(SCARD_E_NOT_TRANSACTED);
			QTest::newRow("Scard_E_Insufficient_Buffer") << PCSC_RETURNCODE(SCARD_E_INSUFFICIENT_BUFFER);
			QTest::newRow("Scard_E_Timeout") << PCSC_RETURNCODE(SCARD_E_TIMEOUT);
			QTest::newRow("Scard_W_Card_Not_Authenticated") << PCSC_RETURNCODE(SCARD_W_CARD_NOT_AUTHENTICATED);

			// Scard_E_Unexpected has the same value as Scard_E_Unsupported_Feature with pcsclite
#ifdef PCSCLITE_VERSION_NUMBER
			QTest::newRow("Scard_E_Unsupported_Feature") << PCSC_RETURNCODE(SCARD_E_UNEXPECTED);
#else
			QTest::newRow("Scard_E_Unexpected") << PCSC_RETURNCODE(SCARD_E_UNEXPECTED);
#endif
			QTest::newRow("Scard_E_Unsupported_Feature") << PCSC_RETURNCODE(SCARD_E_UNSUPPORTED_FEATURE);

			QTest::newRow("UNKNOWN_STATE (0000029a)") << PCSC_RETURNCODE(PCSC_RETURNCODE(666));
			QTest::newRow("Scard_W_Removed_Card") << PCSC_RETURNCODE(PCSC_RETURNCODE(2148532329));
		}


		void checkNames()
		{
			QFETCH(PCSC_RETURNCODE, code);

			QCOMPARE(PcscUtils::toString(code), QString::fromLatin1(QTest::currentDataTag()));
		}


};

QTEST_GUILESS_MAIN(test_PcscUtils)
#include "test_PcscUtils.moc"
