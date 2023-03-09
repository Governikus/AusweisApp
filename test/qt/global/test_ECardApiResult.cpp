/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CardReturnCode.h"
#include "ECardApiResult.h"
#include "LogHandler.h"

#include <QString>
#include <QtTest>

using namespace governikus;

class test_ECardApiResult
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
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void parse()
		{
			const auto& crap = QStringLiteral("crap");
			QCOMPARE(ECardApiResult::parseMajor(crap), ECardApiResult::Major::Unknown);
			QCOMPARE(ECardApiResult::parseMinor(crap), ECardApiResult::Minor::null);

			QVERIFY(!ECardApiResult::isMajor(crap));
			QVERIFY(!ECardApiResult::isMinor(crap));

			const auto& resultMajorOk = QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok");
			const auto& resultMinorNoPermission = QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#noPermission");
			QCOMPARE(ECardApiResult::parseMajor(resultMajorOk), ECardApiResult::Major::Ok);
			QCOMPARE(ECardApiResult::parseMinor(resultMinorNoPermission), ECardApiResult::Minor::AL_No_Permission);

			QVERIFY(ECardApiResult::isMajor(resultMajorOk));
			QVERIFY(ECardApiResult::isMinor(resultMinorNoPermission));

			QCOMPARE(ECardApiResult::parseMinor(QString()), ECardApiResult::Minor::null);
		}


		void createInternalError()
		{
			ECardApiResult result = ECardApiResult(GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity, FailureCode::Reason::Change_Pin_User_Cancelled);
			QCOMPARE(result.getMajor(), ECardApiResult::Major::Error);
			QCOMPARE(result.getMinor(), ECardApiResult::Minor::AL_Internal_Error);
			QCOMPARE(result.getMessage(), QString("The authenticity of your ID card could not be confirmed."));
			QCOMPARE(result.getMessageLang(), QString("en"));
			QCOMPARE(result.getFailureCode(), FailureCode::Reason::Change_Pin_User_Cancelled);
		}


		void createOk()
		{
			ECardApiResult result = ECardApiResult::createOk();
			QCOMPARE(result.getMajor(), ECardApiResult::Major::Ok);
			QCOMPARE(result.getMinor(), ECardApiResult::Minor::null);
			QCOMPARE(result.getMessage(), QString());
			QCOMPARE(result.getMessageLang(), QString("en"));
			QCOMPARE(result.getFailureCode(), std::optional<FailureCode>());
		}


		void logStream()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			qDebug() << ECardApiResult::createOk();

			QCOMPARE(logSpy.count(), 1);
			auto param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok |  | \""));

			logSpy.clear();

			qDebug() << ECardApiResult(GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity);
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#internalError | The authenticity of your ID card could not be confirmed.\""));

			logSpy.clear();

			qDebug() << ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser | The process has been cancelled by the card reader.\""));

			logSpy.clear();

			qDebug() << ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::UNDEFINED));
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError | An unexpected error has occurred during processing.\""));

			logSpy.clear();

			qDebug() << ECardApiResult(GlobalStatus::Code::Workflow_Preverification_Error);
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/il/signature#invalidCertificatePath | Pre-verification failed.\""));

			logSpy.clear();

			qDebug() << ECardApiResult(GlobalStatus::Code::Card_Cancellation_By_User, FailureCode::Reason::Change_Pin_User_Cancelled);
			QCOMPARE(logSpy.count(), 1);
			param = logSpy.takeFirst();
			QVERIFY(param.at(0).toString().contains("Result: \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error | http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser | The process has been cancelled by the card reader. | Change_Pin_User_Cancelled\""));
		}


		void json()
		{
			QByteArray expected;

			expected = "{\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\"}";
			QCOMPARE(bytes(ECardApiResult::createOk().toJson()), expected);

			expected = "{\"description\":\"The process has been cancelled.\","
					   "\"language\":\"en\",\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
					   "\"message\":\"The process has been cancelled by the card reader.\","
					   "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser\"}";
			QCOMPARE(bytes(ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER)).toJson()), expected);

			expected = "{\"description\":\"A Communication error occurred during processing.\","
					   "\"language\":\"en\",\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
					   "\"message\":\"The selected card reader cannot be accessed anymore.\","
					   "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#communicationError\"}";
			QCOMPARE(bytes(ECardApiResult(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible).toJson()), expected);

			expected = "{\"description\":\"The process has been cancelled.\","
					   "\"language\":\"en\",\"major\":\"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\","
					   "\"message\":\"The process has been cancelled by the card reader.\","
					   "\"minor\":\"http://www.bsi.bund.de/ecard/api/1.1/resultminor/sal#cancellationByUser\","
					   "\"reason\":\"Change_Pin_User_Cancelled\"}";
			QCOMPARE(bytes(ECardApiResult(GlobalStatus::Code::Card_Cancellation_By_User, FailureCode::Reason::Change_Pin_User_Cancelled).toJson()), expected);
		}


		void comparison()
		{
			QVERIFY(!(ECardApiResult::createOk() == ECardApiResult(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER))));

			const ECardApiResult& result = ECardApiResult::createOk();
			QVERIFY(result == ECardApiResult(result.toStatus()));
		}


		void convertToStatusMinorNull_data()
		{
			QTest::addColumn<QString>("message");
			QTest::addColumn<ECardApiResult::Origin>("origin");

			QTest::newRow("No msg - client") << QString() << ECardApiResult::Origin::Client;
			QTest::newRow("No msg - server") << QString() << ECardApiResult::Origin::Server;
			QTest::newRow("With msg - client") << QString("Game Over :(") << ECardApiResult::Origin::Client;
			QTest::newRow("With msg - server") << QString("Game Over :(") << ECardApiResult::Origin::Server;
		}


		void convertToStatusMinorNull()
		{
			QFETCH(QString, message);
			QFETCH(ECardApiResult::Origin, origin);

			const ECardApiResult result1(ECardApiResult::Major::Error, ECardApiResult::Minor::null, message, origin);
			QVERIFY(result1.toStatus().isError());

			const QString error("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error");

			const ECardApiResult result2(error, QString(), message, origin);
			QVERIFY(result2.toStatus().isError());

			const ECardApiResult result3(error, "invalid", message, origin);
			QVERIFY(result3.toStatus().isError());

			const ECardApiResult result4(error, "http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError", message, origin);
			QVERIFY(result4.toStatus().isError());
		}


		void convertToStatus_data()
		{
			QTest::addColumn<ECardApiResult::Minor>("minor");

			const QMetaEnum& metaEnum = QMetaEnum::fromType<ECardApiResult::Minor>();
			for (int i = 0; i < metaEnum.keyCount(); i++)
			{
				const auto minor = static_cast<ECardApiResult::Minor>(i);
				const char* name = metaEnum.valueToKey(i);

				if (minor == ECardApiResult::Minor::null)
				{
					continue;
				}

				QTest::newRow(name) << minor;
			}
		}


		void convertToStatus()
		{
			QFETCH(ECardApiResult::Minor, minor);
			const ECardApiResult result(ECardApiResult::Major::Error, minor, "Game Over :(", ECardApiResult::Origin::Client);
			QVERIFY(result.toStatus().isError());
		}


		void convertToStatusAndBackToResult_data()
		{
			QTest::addColumn<ECardApiResult::Minor>("minor");

			const QMetaEnum& metaEnum = QMetaEnum::fromType<ECardApiResult::Minor>();
			for (int i = 0; i < metaEnum.keyCount(); i++)
			{
				const auto minor = static_cast<ECardApiResult::Minor>(i);
				const char* name = metaEnum.valueToKey(i);

				if (minor == ECardApiResult::Minor::null)
				{
					continue;
				}

				QTest::newRow(name) << minor;
			}
		}


		void convertToStatusAndBackToResult()
		{
			QFETCH(ECardApiResult::Minor, minor);
			switch (minor)
			{
				case ECardApiResult::Minor::AL_No_Permission:
				case ECardApiResult::Minor::AL_Parameter_Error:
				case ECardApiResult::Minor::AL_Unknown_API_Function:
				case ECardApiResult::Minor::AL_Not_Initialized:
				case ECardApiResult::Minor::AL_Warning_Connection_Disconnected:
				case ECardApiResult::Minor::AL_Session_Terminated_Warning:
				case ECardApiResult::Minor::DP_Timeout_Error:
				case ECardApiResult::Minor::DP_Unknown_Channel_Handle:
				case ECardApiResult::Minor::DP_Communication_Error:
				case ECardApiResult::Minor::DP_Unknown_Protocol:
				case ECardApiResult::Minor::DP_Unknown_Cipher_Suite:
				case ECardApiResult::Minor::DP_Unknown_Webservice_Binding:
				case ECardApiResult::Minor::DP_Node_Not_Reachable:
				case ECardApiResult::Minor::IFDL_Timeout_Error:
				case ECardApiResult::Minor::IFDL_UnknownSlot:
				case ECardApiResult::Minor::IFDL_CancellationByUser:
				case ECardApiResult::Minor::IFDL_InvalidSlotHandle:
				case ECardApiResult::Minor::IFDL_IFD_SharingViolation:
				case ECardApiResult::Minor::IFDL_Terminal_NoCard:
				case ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch:
				case ECardApiResult::Minor::IFDL_IO_UnknownPINFormat:
				case ECardApiResult::Minor::IL_Signature_InvalidCertificatePath:
				case ECardApiResult::Minor::KEY_KeyGenerationNotPossible:
				case ECardApiResult::Minor::SAL_SecurityConditionNotSatisfied:
				case ECardApiResult::Minor::SAL_MEAC_AgeVerificationFailedWarning:
				case ECardApiResult::Minor::SAL_MEAC_CommunityVerificationFailedWarning:
				case ECardApiResult::Minor::SAL_MEAC_DocumentValidityVerificationFailed:
					QSKIP("No bidirection conversion for this error code intended.");
					break;

				default:
					break;
			}

			const ECardApiResult result_1(ECardApiResult::Major::Error, minor, "Game Over :(", ECardApiResult::Origin::Client);
			QVERIFY(result_1 == ECardApiResult(result_1.toStatus()));

			const ECardApiResult result_2(ECardApiResult::Major::Error, minor, "Game Over :(", ECardApiResult::Origin::Server);
			QVERIFY(!(result_2 == ECardApiResult(result_2.toStatus())));

			const ECardApiResult result_3(ECardApiResult::Major::Error, minor, ECardApiResult::getMessage(minor), ECardApiResult::Origin::Server);
			QVERIFY(result_3 == ECardApiResult(result_3.toStatus()));

			QVERIFY(!(result_1 == result_2));
		}


		void illegalInitialization()
		{
			ECardApiResult internalError = ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Internal_Error, "Lorem ipsum dolor sit amet, consetetur sadipscing elitr.", ECardApiResult::Origin::Client);

			QCOMPARE(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::null, "Lorem ipsum dolor sit amet, consetetur sadipscing elitr.", ECardApiResult::Origin::Client), internalError);
			QCOMPARE(ECardApiResult(ECardApiResult::Major::Ok, ECardApiResult::Minor::AL_Parameter_Error, "Lorem ipsum dolor sit amet, consetetur sadipscing elitr.", ECardApiResult::Origin::Client), internalError);
		}


		void successfulConversionFromStatus_data()
		{
			QTest::addColumn<GlobalStatus::Code>("statusCode");

			const QMetaEnum& metaEnum = QMetaEnum::fromType<GlobalStatus::Code>();
			for (int i = 0; i < metaEnum.keyCount(); i++)
			{
				const auto statusCode = static_cast<GlobalStatus::Code>(i);
				const char* name = metaEnum.valueToKey(i);
				QTest::newRow(name) << statusCode;
			}
		}


		void successfulConversionFromStatus()
		{
			QFETCH(GlobalStatus::Code, statusCode);

			ECardApiResult::fromStatus(statusCode);
		}


		void getMessage_data()
		{
			QTest::addColumn<ECardApiResult::Minor>("minor");
			QTest::addColumn<QString>("message");

			QTest::newRow("unknownError") << ECardApiResult::Minor::AL_Unknown_Error << tr("An unexpected error has occurred during processing.");
			QTest::newRow("noPermission") << ECardApiResult::Minor::AL_No_Permission << tr("Use of the function by the client application is not permitted.");
			QTest::newRow("internalError") << ECardApiResult::Minor::AL_Internal_Error << tr("An internal error has occurred during processing.");
			QTest::newRow("parameterError") << ECardApiResult::Minor::AL_Parameter_Error << tr("There was some problem with a provided or omitted parameter.");
			QTest::newRow("unknownApiFunction") << ECardApiResult::Minor::AL_Unknown_API_Function << tr("The API function is unknown.");
			QTest::newRow("notInitialized") << ECardApiResult::Minor::AL_Not_Initialized << tr("The framework or layer has not been initialized.");
			QTest::newRow("warningDisconnected") << ECardApiResult::Minor::AL_Warning_Connection_Disconnected << tr("The active session has been terminated.");
			QTest::newRow("sessionTerminated") << ECardApiResult::Minor::AL_Session_Terminated_Warning << tr("The active session has been terminated.");
			QTest::newRow("communicationError") << ECardApiResult::Minor::AL_Communication_Error << tr("A Communication error occurred during processing.");
			QTest::newRow("timeoutError") << ECardApiResult::Minor::DP_Timeout_Error << tr("The operation was terminated as the set time was exceeded.");
			QTest::newRow("unknownChannelHandle") << ECardApiResult::Minor::DP_Unknown_Channel_Handle << tr("The operation was aborted as an invalid channel handle was used.");
			QTest::newRow("dpCommunicationError") << ECardApiResult::Minor::DP_Communication_Error << tr("A Communication error occurred during processing.");
			QTest::newRow("channelEstablishmentFailed") << ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed << tr("A trusted channel could not be opened.");
			QTest::newRow("unknownProtocol") << ECardApiResult::Minor::DP_Unknown_Protocol << tr("The operation was aborted as an unknown protocol was used.");
			QTest::newRow("unknownCipherSuite") << ECardApiResult::Minor::DP_Unknown_Cipher_Suite << tr("The operation was aborted as an unknown cipher suite was used.");
			QTest::newRow("unknownWebserviceBinding") << ECardApiResult::Minor::DP_Unknown_Webservice_Binding << tr("The operation was aborted as an unknown web service binding was used.");
			QTest::newRow("NodeNotReachable") << ECardApiResult::Minor::DP_Node_Not_Reachable << tr("A Communication error occurred during processing.");
			QTest::newRow("IfdlTimeoutError") << ECardApiResult::Minor::IFDL_Timeout_Error << tr("The operation was terminated as the set time was exceeded.");
			QTest::newRow("terminalNoCard") << ECardApiResult::Minor::IFDL_Terminal_NoCard << tr("The card is missing or was removed.");
			QTest::newRow("repeatedDataMismatch") << ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch << tr("The new PIN and the confirmation do not match.");
			QTest::newRow("unknownPinFormat") << ECardApiResult::Minor::IFDL_IO_UnknownPINFormat << tr("The format of the PIN is wrong.");
			QTest::newRow("keyGenerationNotPossible") << ECardApiResult::Minor::KEY_KeyGenerationNotPossible << tr("Signature certificate key generation is not possible.");
			QTest::newRow("cancellationByUser") << ECardApiResult::Minor::SAL_Cancellation_by_User << tr("The process has been cancelled.");
			QTest::newRow("invalidCertificatePath") << ECardApiResult::Minor::IL_Signature_InvalidCertificatePath << tr("One or more certificate checks failed. The operation will be aborted due to security reasons.");
			QTest::newRow("invalidKey") << ECardApiResult::Minor::SAL_Invalid_Key << tr("This action cannot be performed. The eID function of your ID card is not activated.");
			QTest::newRow("securityConditionNotSatisfied") << ECardApiResult::Minor::SAL_SecurityConditionNotSatisfied << tr("The authenticity of your ID card could not be verified. Please make sure that you are using a genuine ID card. Please note that test applications require the use of a test ID card.");
			QTest::newRow("ageVerificationFailed") << ECardApiResult::Minor::SAL_MEAC_AgeVerificationFailedWarning << tr("The age verification failed.");
			QTest::newRow("comunityVerificationFailed") << ECardApiResult::Minor::SAL_MEAC_CommunityVerificationFailedWarning << tr("The community verification failed.");
			QTest::newRow("documentValidityVerificationFailed") << ECardApiResult::Minor::SAL_MEAC_DocumentValidityVerificationFailed << tr("The ID card is invalid or disabled.");
			QTest::newRow("null") << ECardApiResult::Minor::null << QString();
			QTest::newRow("default") << ECardApiResult::Minor::IFDL_IFD_SharingViolation << QString();
		}


		void getMessage()
		{
			QFETCH(ECardApiResult::Minor, minor);
			QFETCH(QString, message);

			ECardApiResult result(GlobalStatus::Code::Unknown_Error);

			QCOMPARE(result.getMessage(minor), message);
		}


};

QTEST_GUILESS_MAIN(test_ECardApiResult)
#include "test_ECardApiResult.moc"
