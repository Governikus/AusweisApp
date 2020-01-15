/*!
 * \brief Unit tests for \ref GlobalStatus.
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "GlobalStatus.h"

#include <QtTest>

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
			GlobalStatus status2(GlobalStatus::Code::Workflow_Cancellation_By_User, QStringList(), Origin::Client);
			GlobalStatus status3(GlobalStatus::Code::Card_Cancellation_By_User, QStringList(), Origin::Client);
			GlobalStatus status4(GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User, QStringList(), Origin::Client);

			QVERIFY(!status1.isCancellationByUser());
			QVERIFY(status2.isCancellationByUser());
			QVERIFY(status3.isCancellationByUser());
			QVERIFY(status4.isCancellationByUser());
		}


		void test_ToDescriptionError_data()
		{
			QTest::addColumn<GlobalStatus::Code>("code");
			QTest::addColumn<QString>("message");

			QTest::newRow("inProgress") << GlobalStatus::Code::Workflow_AlreadyInProgress_Error << tr("Cannot start authentication. An operation is already active.");
			QTest::newRow("cardRemoved") << GlobalStatus::Code::Workflow_Card_Removed << tr("The connection to the ID card has been lost. The process was aborted.");
			QTest::newRow("unknownPaos") << GlobalStatus::Code::Workflow_Unknown_Paos_From_EidServer << tr("The program received an unknown message from the server.");
			QTest::newRow("unexpectedMessage") << GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer << tr("The program received an unexpected message from the server.");
			QTest::newRow("preverificationDevelopermode") << GlobalStatus::Code::Workflow_Preverification_Developermode_Error << tr("Using the developer mode is only allowed in a test environment.");
			QTest::newRow("noUniqueAtCvc") << GlobalStatus::Code::Workflow_No_Unique_AtCvc << tr("No unique AT CVC");
			QTest::newRow("noUniqueDvCvc") << GlobalStatus::Code::Workflow_No_Unique_DvCvc << tr("No unique DV CVC");
			QTest::newRow("noPermission") << GlobalStatus::Code::Workflow_No_Permission_Error << tr("Authentication failed.");
			QTest::newRow("certificateNoDescription") << GlobalStatus::Code::Workflow_Certificate_No_Description << tr("No certificate description available.");
			QTest::newRow("certificateNoUrl") << GlobalStatus::Code::Workflow_Certificate_No_Url_In_Description << tr("No subject url available in certificate description.");
			QTest::newRow("hashError") << GlobalStatus::Code::Workflow_Certificate_Hash_Error << tr("The certificate description does not match the certificate.");
			QTest::newRow("certificateSop") << GlobalStatus::Code::Workflow_Certificate_Sop_Error << tr("The subject URL in the certificate description and the TCToken URL don't satisfy the same origin policy.");
			QTest::newRow("wrongParameter") << GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation << tr("Application was invoked with wrong parameters.");
			QTest::newRow("incompleteInformation") << GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided << tr("The server provided no or incomplete information. Your personal data could not be read out.");
			QTest::newRow("deviceConnection") << GlobalStatus::Code::Workflow_Bluetooth_Reader_Connection_Error << tr("An error occurred while connecting to a bluetooth card reader.");
			QTest::newRow("deviceScan") << GlobalStatus::Code::Workflow_Reader_Device_Scan_Error << tr("An error occurred while scanning for a bluetooth card reader.");
			QTest::newRow("abnormalClose") << GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose << tr("The smartphone as card reader (SaC) connection was aborted.");
			QTest::newRow("invalidRequest") << GlobalStatus::Code::RemoteConnector_InvalidRequest << tr("Smartphone as card reader (SaC) connection request was invalid.");
			QTest::newRow("noSupportedApiLevel") << GlobalStatus::Code::RemoteConnector_NoSupportedApiLevel << tr("Your smartphone as card reader (SaC) version is incompatible with the local version. Please install the latest AusweisApp2 version on both your smartphone and your computer.");
			QTest::newRow("connectionTimeout") << GlobalStatus::Code::RemoteConnector_ConnectionTimeout << tr("A timeout occurred while trying to establish a connection to the smartphone as card reader (SaC).");
			QTest::newRow("connectionError") << GlobalStatus::Code::RemoteConnector_ConnectionError << tr("An error occurred while trying to establish a connection to the smartphone as card reader (SaC).");
			QTest::newRow("hostRefused") << GlobalStatus::Code::RemoteConnector_RemoteHostRefusedConnection << tr("The smartphone to be paired has rejected the connection. Please check the pairing code. If no pairing code is shown activate the pairing mode.");
			QTest::newRow("cannotSaveFile") << GlobalStatus::Code::Downloader_Cannot_Save_File << tr("Cannot save file.");
			QTest::newRow("dataCorrupted") << GlobalStatus::Code::Downloader_Data_Corrupted << tr("Received data were corrupted.");
		}


		void test_ToDescriptionError()
		{
			QFETCH(GlobalStatus::Code, code);
			QFETCH(QString, message);

			GlobalStatus status(code, QStringList(), Origin::Client);

			QCOMPARE(status.toErrorDescription(false), message);
		}


};

QTEST_GUILESS_MAIN(test_GlobalStatus)
#include "test_GlobalStatus.moc"
