/*!
 * test_result.h
 *
 * \brief Unit tests for \ref result
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "LogHandler.h"
#include "Result.h"

#include <QString>
#include <QtTest/QtTest>

using namespace governikus;

class test_result
	: public QObject
{
	Q_OBJECT

	static QByteArray bytes(const QJsonObject& pObj)
	{
		return QJsonDocument(pObj).toJson(QJsonDocument::Compact);
	}


	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void parse()
		{
			QCOMPARE(Result::parseMajor("crap"), Result::Major::Unknown);
			QCOMPARE(Result::parseMinor("crap"), GlobalStatus::Code::Unknown_Error);

			QVERIFY(!Result::isMajor("crap"));
			QVERIFY(!Result::isMinor("crap"));

			QCOMPARE(Result::parseMajor("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"), Result::Major::Ok);
			QCOMPARE(Result::parseMinor("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#noPermission"),
					GlobalStatus::Code::Paos_Error_AL_No_Permission);

			QVERIFY(Result::isMajor("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QVERIFY(Result::isMinor("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#noPermission"));
		}


		void createInternalError()
		{
			Result result = Result(GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity);
			QCOMPARE(result.getMajor(), Result::Major::Error);
			QCOMPARE(result.getMinor(), GlobalStatus::Code::Paos_Error_AL_Internal_Error);
			QCOMPARE(result.getMessage(), QString("The authenticity of your ID card could not be confirmed."));
			QCOMPARE(result.getMessageLang(), QString("en"));
		}


		void createOk()
		{
			Result result = Result::createOk();
			QCOMPARE(result.getMajor(), Result::Major::Ok);
			QCOMPARE(result.getMinor(), GlobalStatus::Code::Unknown_Error);
			QCOMPARE(result.getMessage(), QString());
			QCOMPARE(result.getMessageLang(), QString("en"));
		}


		void logStream()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			qDebug() << Result::createOk();

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok |  | \""));

			spy.clear();

			qDebug() << Result(GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity);
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#internalError | The authenticity of your ID card could not be confirmed.\""));

			spy.clear();

			qDebug() << Result(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser | The process was cancelled by the user.\""));

			spy.clear();

			qDebug() << Result(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::UNDEFINED));
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError | An unexpected error has occurred during processing.\""));

			spy.clear();

			qDebug() << Result(GlobalStatus::Code::Workflow_Preverification_Error);
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#certificateChainInterrupted | Pre-verification failed.\""));
		}


		void json()
		{
			QByteArray expected;

			expected = "{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"}";
			QCOMPARE(bytes(Result::createOk().toJson()), expected);

			expected = "{\"description\":\"The operation was aborted due to cancellation by user.\","
					   "\"language\":\"en\",\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
					   "\"message\":\"The process was cancelled by the user.\","
					   "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser\"}";
			QCOMPARE(bytes(Result(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER)).toJson()), expected);

			expected = "{\"description\":\"A Communication error occurred during processing.\","
					   "\"language\":\"en\",\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
					   "\"message\":\"The selected card reader cannot be accessed anymore.\","
					   "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#communicationError\"}";
			QCOMPARE(bytes(Result(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible).toJson()), expected);
		}


		void comparison()
		{
			QVERIFY(!(Result::createOk() == Result(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER))));

			const Result& result = Result::createOk();
			QVERIFY(result == Result(result.toStatus()));
		}


		void conversion_data()
		{
			QTest::addColumn<GlobalStatus::Code>("minor");

			const QMetaEnum& metaEnum = QMetaEnum::fromType<GlobalStatus::Code>();
			for (int i = 0; i < metaEnum.keyCount(); i++)
			{
				const GlobalStatus::Code minor = static_cast<GlobalStatus::Code>(i);
				const char* name = metaEnum.valueToKey(i);
				const QString check = QString::fromStdString(name);

				if (minor == GlobalStatus::Code::Paos_Unexpected_Warning || !check.startsWith(QLatin1String("Paos_")))
				{
					continue;
				}

				QTest::newRow(name) << minor;
			}
		}


		void conversion()
		{
			QFETCH(GlobalStatus::Code, minor);

			const Result result_1(Result::Major::Error, minor, "Game Over :(", Origin::Client);
			QVERIFY(result_1 == Result(result_1.toStatus()));

			const Result result_2(Result::Major::Error, minor, "Game Over :(", Origin::Server);
			QVERIFY(!(result_2 == Result(result_2.toStatus())));

			const Result result_3(Result::Major::Error, minor, Result::getMessage(minor), Origin::Server);
			QVERIFY(result_3 == Result(result_3.toStatus()));

			QVERIFY(!(result_1 == result_2));
		}


};

QTEST_GUILESS_MAIN(test_result)
#include "test_result.moc"
