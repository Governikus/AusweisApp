/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"
#include "context/PersonalizationContext.h"
#include "states/StateStartPaosPersonalizationResponse.h"

#include <QSharedPointer>
#include <QTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(std::optional<FailureCode>)


class test_StateStartPaosPersonalizationResponse
	: public QObject
{
	Q_OBJECT

	private:
		QByteArray createResult(const QByteArray& pMajor, const QByteArray& pMinor = QByteArray())
		{
			const QByteArray resultMajor = "<ns2:ResultMajor>http://www.bsi.bund.de/ecard/api/1.1/resultmajor#" + pMajor + "</ns2:ResultMajor>";
			QByteArray resultMinor;

			if (!pMinor.isEmpty())
			{
				resultMinor = "<ns2:ResultMinor>http://www.bsi.bund.de/ecard/api/1.1/resultminor/" + pMinor + "</ns2:ResultMinor>";
			}

			return "<ns2:Result>" + resultMajor + resultMinor + "</ns2:Result>";
		}


		QByteArray createOutputs(int pFinalizeStatus, const QByteArray& pRemainingDays, const QByteArray& pRemainingAttempts = QByteArray(), const QByteArray& pBlockingCode = QByteArray())
		{
			QByteArray outputs = "<statusCode>" + QByteArray::number(pFinalizeStatus) + "</statusCode>";
			outputs += "<remainingDays>" + pRemainingDays + "</remainingDays>";

			if (!pRemainingAttempts.isEmpty())
			{
				outputs += "<remainingAttempts>" + pRemainingAttempts + "</remainingAttempts>";
			}

			if (!pBlockingCode.isEmpty())
			{
				outputs += "<blockingCode>" + pBlockingCode + "</blockingCode>";
			}

			return R"(
				<ns2:OptionalOutputs>
					<AdditionalInformation xmlns="urn:de:governikus:idpersonalisation:tech:schema">
				)" + outputs + R"(
					</AdditionalInformation>
				</ns2:OptionalOutputs>
				)";
		}

	private Q_SLOTS:
		void noPaosResponse()
		{
			auto context = QSharedPointer<PersonalizationContext>::create(QString());
			StateStartPaosPersonalizationResponse state(context);

			QSignalSpy spy(&state, &StateStartPaosPersonalizationResponse::fireAbort);
			state.run();
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Start_Paos_Response_Personalization_Empty);
			QCOMPARE(spy.count(), 1);
		}


		void testPersonalization_data()
		{
			const auto& dateFormat = "d. MMMM yyyy"_L1;

			QTest::addColumn<int>("finalizeStatus");
			QTest::addColumn<QString>("restrictionDate");
			QTest::addColumn<int>("remainingAttempts");
			QTest::addColumn<QString>("blockingCode");
			QTest::addColumn<ECardApiResult::Major>("resultMajor");
			QTest::addColumn<ECardApiResult::Minor>("resultMinor");
			QTest::addColumn<GlobalStatus::Code>("statusCode");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");
			QTest::addColumn<QByteArray>("resultContent");
			QTest::addColumn<QByteArray>("outputsContent");

			QTest::addRow("Success") << 1 << QDate::currentDate().addDays(30).toString(dateFormat) << 9 << "LASTWAGEN"
									 << ECardApiResult::Major::Ok << ECardApiResult::Minor::null
									 << GlobalStatus::Code::No_Error
									 << std::optional<FailureCode>()
									 << createResult("ok") << createOutputs(1, "30", "9", "LASTWAGEN");
			QTest::addRow("Failure - resultMinor missing") << 2 << QDate::currentDate().addDays(13).toString(dateFormat) << -1 << ""
														   << ECardApiResult::Major::Error << ECardApiResult::Minor::null
														   << GlobalStatus::Code::Paos_Error_AL_Unknown_Error
														   << std::optional<FailureCode>(FailureCode::Reason::Start_Paos_Response_Personalization_Invalid)
														   << createResult("error") << createOutputs(2, "-13");
			QTest::addRow("No Permission") << -3 << QDate::currentDate().addDays(13).toString(dateFormat) << -1 << ""
										   << ECardApiResult::Major::Error << ECardApiResult::Minor::AL_No_Permission
										   << GlobalStatus::Code::Workflow_Smart_eID_Personalization_Denied
										   << std::optional<FailureCode>(FailureCode::Reason::Start_Paos_Response_Personalization_Invalid)
										   << createResult("error", "al/common#noPermission") << createOutputs(-3, "-13");
			QTest::addRow("No OptionalOutputs") << 0 << QDate::currentDate().addDays(1).toString(dateFormat) << -1 << ""
												<< ECardApiResult::Major::Error << ECardApiResult::Minor::null
												<< GlobalStatus::Code::Paos_Error_AL_Unknown_Error
												<< std::optional<FailureCode>(FailureCode::Reason::Start_Paos_Response_Personalization_Invalid)
												<< createResult("error", "al#noPermission") << QByteArray();
		}


		void testPersonalization()
		{
			QFETCH(int, finalizeStatus);
			QFETCH(QString, restrictionDate);
			QFETCH(int, remainingAttempts);
			QFETCH(QString, blockingCode);
			QFETCH(ECardApiResult::Major, resultMajor);
			QFETCH(ECardApiResult::Minor, resultMinor);
			QFETCH(GlobalStatus::Code, statusCode);
			QFETCH(std::optional<FailureCode>, failureCode);
			QFETCH(QByteArray, resultContent);
			QFETCH(QByteArray, outputsContent);

			QByteArray content = TestFileHelper::readFile(":/paos/StartPAOSResponse_template.xml"_L1);
			content.replace("<!-- RESULT -->", resultContent);
			content.replace("<!-- OPTIONALOUTPUTS -->", outputsContent);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());
			context->setStartPaosResponse(QSharedPointer<StartPaosResponse>::create(content));

			StateStartPaosPersonalizationResponse state(context);
			QSignalSpy spyAbort(&state, &StateStartPaosPersonalizationResponse::fireAbort);
			QSignalSpy spyContinue(&state, &StateStartPaosPersonalizationResponse::fireContinue);
			state.run();

			QCOMPARE(context->getFinalizeStatus(), finalizeStatus);
			QCOMPARE(context->getRestrictionDate(), restrictionDate);
			QCOMPARE(context->getRemainingAttempts(), remainingAttempts);
			QCOMPARE(context->getBlockingCode(), blockingCode);
			QCOMPARE(context->getStartPaosResult().getMajor(), resultMajor);
			QCOMPARE(context->getStartPaosResult().getMinor(), resultMinor);
			QCOMPARE(context->getStatus().getStatusCode(), statusCode);
			QCOMPARE(context->getFailureCode(), failureCode);
			if (resultMajor == ECardApiResult::Major::Ok)
			{
				QCOMPARE(spyContinue.count(), 1);
			}
			else
			{
				QCOMPARE(spyAbort.count(), 1);
			}
		}


};

QTEST_GUILESS_MAIN(test_StateStartPaosPersonalizationResponse)
#include "test_StateStartPaosPersonalizationResponse.moc"
