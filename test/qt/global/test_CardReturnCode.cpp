/*!
 * \brief Unit tests for return codes.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QPair>
#include <QString>
#include <QtTest>

#include "Result.h"

using namespace governikus;


class test_CardReturnCode
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void check_errorCodeToMessage()
		{
			QCOMPARE(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER).toErrorDescription(),
					QString("The process was cancelled by the user."));
		}


		void check_errorCodeToResult()
		{
			QCOMPARE(Result(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER)).getMinor(), GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User);
		}


		void check_errorCodeToError()
		{
			const Result& result = Result(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));
			QCOMPARE(result.getMinor(), GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User);
			QCOMPARE(result.getMessage(), QString("The process was cancelled by the user."));
		}


		void checkConsistency()
		{
			for (auto returnCode : Enum<CardReturnCode>::getList())
			{
				QVERIFY(!CardReturnCodeUtil::toGlobalStatus(returnCode).toErrorDescription().isEmpty());
			}
		}


};

QTEST_GUILESS_MAIN(test_CardReturnCode)
#include "test_CardReturnCode.moc"
