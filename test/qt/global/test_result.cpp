/*!
 * test_result.h
 *
 * \brief Unit tests for \ref result
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "LogHandler.h"
#include "Result.h"

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
			QCOMPARE(Result::parseMajor("crap"), Result::Major::null);
			QCOMPARE(Result::parseMinor("crap"), Result::Minor::null);

			QVERIFY(!Result::isMajor("crap"));
			QVERIFY(!Result::isMinor("crap"));

			QCOMPARE(Result::parseMajor("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"), Result::Major::Ok);
			QCOMPARE(Result::parseMinor("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#noPermission"),
					Result::Minor::AL_No_Permission);

			QVERIFY(Result::isMajor("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QVERIFY(Result::isMinor("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#noPermission"));
		}


		void createInternalError()
		{
			Result result = Result::createInternalError();
			QCOMPARE(result.getMajor(), Result::Major::Error);
			QCOMPARE(result.getMinor(), Result::Minor::AL_Internal_Error);
			QCOMPARE(result.getMessage(), QString());
			QCOMPARE(result.getMessageLang(), QString("en"));
		}


		void createInternalErrorMessage()
		{
			Result result = Result::createInternalError("test message");
			QCOMPARE(result.getMajor(), Result::Major::Error);
			QCOMPARE(result.getMinor(), Result::Minor::AL_Internal_Error);
			QCOMPARE(result.getMessage(), QString("test message"));
			QCOMPARE(result.getMessageLang(), QString("en"));
		}


		void createOk()
		{
			Result result = Result::createOk();
			QCOMPARE(result.getMajor(), Result::Major::Ok);
			QCOMPARE(result.getMinor(), Result::Minor::null);
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

			qDebug() << Result::createInternalError("dummy message");
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#internalError | dummy message\""));

			spy.clear();

			qDebug() << Result::createCancelByUserError();
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser | The process was cancelled by the user.\""));

			spy.clear();

			qDebug() << Result::createError(ReturnCode::UNDEFINED);
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError | An unknown error occurred: UNDEFINED\""));

			spy.clear();

			qDebug() << Result::createCertChainInterruptedError("");
			QCOMPARE(spy.count(), 1);
			param = spy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#certificateChainInterrupted | \""));
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
			QCOMPARE(bytes(Result::createCancelByUserError().toJson()), expected);

			expected = "{\"description\":\"A Communication error occurred during processing.\","
					   "\"language\":\"en\",\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
					   "\"message\":\"BAMM!\","
					   "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#communicationError\"}";
			QCOMPARE(bytes(Result::createCommunicationError("BAMM!").toJson()), expected);
		}


};

QTEST_GUILESS_MAIN(test_result)
#include "test_result.moc"
