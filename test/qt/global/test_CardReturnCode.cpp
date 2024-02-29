/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for return codes.
 */

#include "CardReturnCode.h"
#include "ECardApiResult.h"

#include <QPair>
#include <QString>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_CardReturnCode
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void check_errorCodeToMessage()
		{
			QCOMPARE(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER).toErrorDescription(),
					"The process has been cancelled by the card reader."_L1);
		}


		void check_errorCodeToResult()
		{
			QCOMPARE(ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER)).getMinor(), ECardApiResult::Minor::SAL_Cancellation_by_User);
		}


		void check_errorCodeToError()
		{
			const ECardApiResult& result = ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));
			QCOMPARE(result.getMinor(), ECardApiResult::Minor::SAL_Cancellation_by_User);
			QCOMPARE(result.getMessage(), "The process has been cancelled by the card reader."_L1);
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
