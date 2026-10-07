/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "GlobalStatus.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_GlobalStatus
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Is()
		{
			GlobalStatus status;

			QVERIFY(status.is(GlobalStatus::Code::Unknown_Error));
			QVERIFY(!status.is(GlobalStatus::Code::Downloader_Cannot_Save_File));
		}


		void test_IsCancellationByUser()
		{
			GlobalStatus status1;
			GlobalStatus status2(GlobalStatus::Code::Workflow_Cancellation_By_User, Origin::Client);
			GlobalStatus status3(GlobalStatus::Code::Card_Cancellation_By_User, Origin::Client);
			GlobalStatus status4(GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User, Origin::Client);

			QVERIFY(!status1.isCancellationByUser());
			QVERIFY(status2.isCancellationByUser());
			QVERIFY(status3.isCancellationByUser());
			QVERIFY(status4.isCancellationByUser());
		}


		void test_ToDescriptionError_data()
		{
			QTest::addColumn<GlobalStatus::Code>("code");
			QTest::addColumn<GlobalStatus::ExternalInfoMap>("externalInfoMap");
			QTest::addColumn<QString>("message");

			QTest::newRow("inProgress") << GlobalStatus::Code::Workflow_AlreadyInProgress_Error << GlobalStatus::ExternalInfoMap() << tr("Cannot start authentication. An operation is already active.");
			QTest::newRow("cardRemoved") << GlobalStatus::Code::Workflow_Card_Removed << GlobalStatus::ExternalInfoMap() << tr("Restart the authentication process and make sure that the position of the ID card does not change during the reading process.");
			QTest::newRow("unknownPaos") << GlobalStatus::Code::Workflow_Unknown_Paos_From_EidServer << GlobalStatus::ExternalInfoMap() << tr("The program received an unknown message from the server.");
			QTest::newRow("unexpectedMessage") << GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer << GlobalStatus::ExternalInfoMap() << tr("The program received an unexpected message from the server.");
			QTest::newRow("preverificationDevelopermode") << GlobalStatus::Code::Workflow_Preverification_Developermode_Error << GlobalStatus::ExternalInfoMap() << tr("Using the developer mode is only allowed in a test environment.");
			QTest::newRow("noUniqueAtCvc") << GlobalStatus::Code::Workflow_No_Unique_AtCvc << GlobalStatus::ExternalInfoMap() << tr("No unique AT CVC");
			QTest::newRow("noUniqueDvCvc") << GlobalStatus::Code::Workflow_No_Unique_DvCvc << GlobalStatus::ExternalInfoMap() << tr("No unique DV CVC");
			QTest::newRow("noPermission") << GlobalStatus::Code::Workflow_No_Permission_Error << GlobalStatus::ExternalInfoMap() << tr("Authentication failed.");
			QTest::newRow("certificateNoDescription") << GlobalStatus::Code::Workflow_Certificate_No_Description << GlobalStatus::ExternalInfoMap() << tr("No certificate description available.");
			QTest::newRow("certificateNoUrl") << GlobalStatus::Code::Workflow_Certificate_No_Url_In_Description << GlobalStatus::ExternalInfoMap() << tr("No subject url available in certificate description.");
			QTest::newRow("hashError") << GlobalStatus::Code::Workflow_Certificate_Hash_Error << GlobalStatus::ExternalInfoMap() << tr("The certificate description does not match the certificate.");
			QTest::newRow("certificateSop") << GlobalStatus::Code::Workflow_Certificate_Sop_Error << GlobalStatus::ExternalInfoMap() << tr("The subject URL in the certificate description and the TCToken URL do not satisfy the same origin policy.");
			QTest::newRow("wrongParameter") << GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation << GlobalStatus::ExternalInfoMap() << tr("Application was invoked with wrong parameters.");
			QTest::newRow("incompleteInformation") << GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided << GlobalStatus::ExternalInfoMap() << tr("The server provided no or incomplete information. Your personal data could not be read out.");
			QTest::newRow("abnormalClose") << GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose << GlobalStatus::ExternalInfoMap() << tr("The smartphone as card reader (SaC) connection was aborted.");
			QTest::newRow("invalidRequest") << GlobalStatus::Code::IfdConnector_InvalidRequest << GlobalStatus::ExternalInfoMap() << tr("Smartphone as card reader (SaC) connection request was invalid.");
			QTest::newRow("noSupportedApiLevel") << GlobalStatus::Code::IfdConnector_NoSupportedApiLevel << GlobalStatus::ExternalInfoMap() << tr("Your smartphone as card reader (SaC) version is incompatible with the local version. Please install the latest %1 version on both your smartphone and your computer.").arg(QCoreApplication::applicationName());
			QTest::newRow("connectionError") << GlobalStatus::Code::IfdConnector_ConnectionError << GlobalStatus::ExternalInfoMap() << tr("An error occurred while trying to establish a connection to the smartphone as card reader (SaC).");
			QTest::newRow("hostRefused") << GlobalStatus::Code::IfdConnector_RemoteHostRefusedConnection << GlobalStatus::ExternalInfoMap() << tr("The smartphone to be paired has rejected the connection. Please check the pairing code.");
			QTest::newRow("fileNotFound") << GlobalStatus::Code::Downloader_File_Not_Found << GlobalStatus::ExternalInfoMap() << tr("File not found.");
			QTest::newRow("cannotSaveFile") << GlobalStatus::Code::Downloader_Cannot_Save_File << GlobalStatus::ExternalInfoMap() << tr("Cannot save file.");
			QTest::newRow("dataCorrupted") << GlobalStatus::Code::Downloader_Data_Corrupted << GlobalStatus::ExternalInfoMap() << tr("Received data were corrupted.");
			QTest::newRow("missingPlatform") << GlobalStatus::Code::Downloader_Missing_Platform << GlobalStatus::ExternalInfoMap() << tr("Received data does not contain data for the current platform.");
			QTest::newRow("downloadAborted") << GlobalStatus::Code::Downloader_Aborted << GlobalStatus::ExternalInfoMap() << tr("Download aborted.");
			QTest::newRow("updateFailed") << GlobalStatus::Code::Update_Execution_Failed << GlobalStatus::ExternalInfoMap() << tr("A new process to start the update could not be launched.");
			QTest::newRow("genericServerErrorOneValueHtml") << GlobalStatus::Code::Paos_Generic_Server_Error << GlobalStatus::ExternalInfoMap({
						{GlobalStatus::ExternalInformation::ECARDAPI_SERVERMESSAGE, "<a href=\"https://www.governikus.de\">https://www.governikus.de</a>"_L1}
					}) << tr("&lt;a href=&quot;https://www.governikus.de&quot;&gt;https://www.governikus.de&lt;/a&gt;");
			QTest::newRow("genericServerErrorTwoValuesHtml") << GlobalStatus::Code::Paos_Generic_Server_Error << GlobalStatus::ExternalInfoMap({
						{GlobalStatus::ExternalInformation::ECARDAPI_ERROR, "The operation was terminated as the set time was exceeded."_L1},
						{GlobalStatus::ExternalInformation::ECARDAPI_SERVERMESSAGE, "<a href=\"https://www.governikus.de\">https://www.governikus.de</a>"_L1}
					}) << tr("ECARDAPI_ERROR: The operation was terminated as the set time was exceeded.; ECARDAPI_SERVERMESSAGE: &lt;a href=&quot;https://www.governikus.de&quot;&gt;https://www.governikus.de&lt;/a&gt;");
		}


		void test_ToDescriptionError()
		{
			QFETCH(GlobalStatus::Code, code);
			QFETCH(GlobalStatus::ExternalInfoMap, externalInfoMap);
			QFETCH(QString, message);

			GlobalStatus status(code, externalInfoMap, Origin::Client);

			QCOMPARE(status.toErrorDescription(false), message);
		}


};

QTEST_GUILESS_MAIN(test_GlobalStatus)
#include "test_GlobalStatus.moc"
