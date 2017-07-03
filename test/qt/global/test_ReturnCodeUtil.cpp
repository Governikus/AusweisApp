/*!
 * \brief Unit tests for return codes.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReturnCodeUtil.h"

#include <QPair>
#include <QString>
#include <QtTest>

using namespace governikus;


class test_ReturnCodeUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void check_errorCodeToString()
		{
			QCOMPARE(ReturnCodeUtil::toString(ReturnCode::CANCELLATION_BY_USER),
					QString("The process was cancelled by the user."));
		}


		void check_errorCodeToMessage()
		{
			QCOMPARE(ReturnCodeUtil::toMessage(ReturnCode::CANCELLATION_BY_USER),
					QString("The process was cancelled by the user."));
		}


		void check_errorCodeToResult()
		{
			QCOMPARE(ReturnCodeUtil::toResult(ReturnCode::CANCELLATION_BY_USER),
					Result::Minor::SAL_Cancellation_by_User);
		}


		void check_errorCodeToError()
		{
			auto error = ReturnCodeUtil::toError(ReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(error.first, Result::Minor::SAL_Cancellation_by_User);
			QCOMPARE(error.second, QString("The process was cancelled by the user."));
		}


		void checkConsistency()
		{
			for (auto returnCode : EnumReturnCode::getList())
			{
				QVERIFY(!ReturnCodeUtil::toString(returnCode).isEmpty());
				QVERIFY(!ReturnCodeUtil::toMessage(returnCode).isEmpty());
			}
		}


};

QTEST_GUILESS_MAIN(test_ReturnCodeUtil)
#include "test_ReturnCodeUtil.moc"
