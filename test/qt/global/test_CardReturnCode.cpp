/*!
 * \brief Unit tests for return codes.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CardReturnCode.h"
#include "ECardApiResult.h"

#include <QPair>
#include <QString>
#include <QtTest>

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
			QCOMPARE(ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER)).getMinor(), ECardApiResult::Minor::SAL_Cancellation_by_User);
		}


		void check_errorCodeToError()
		{
			const ECardApiResult& result = ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));
			QCOMPARE(result.getMinor(), ECardApiResult::Minor::SAL_Cancellation_by_User);
			QCOMPARE(result.getMessage(), QString("The process was cancelled by the user."));
		}


		void checkConsistency()
		{
			const auto list = Enum<CardReturnCode>::getList();
			for (auto returnCode : list)
			{
				QVERIFY(!CardReturnCodeUtil::toGlobalStatus(returnCode).toErrorDescription().isEmpty());
			}
		}


};

QTEST_GUILESS_MAIN(test_CardReturnCode)
#include "test_CardReturnCode.moc"
