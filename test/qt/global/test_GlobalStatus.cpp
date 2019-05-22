/*!
 * \brief Unit tests for \ref GlobalStatus.
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
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

			QTest::newRow("inProgress") << GlobalStatus::Code::Workflow_AlreadyInProgress_Error << tr("Cannot start authentication. An operation is already in progress.");
			QTest::newRow("cardRemoved") << GlobalStatus::Code::Workflow_Card_Removed << tr("The ID card has been removed. The process is aborted.");
			QTest::newRow("unknownPaos") << GlobalStatus::Code::Workflow_Unknown_Paos_Form_EidServer << tr("The program received an unknown message from the server.");
			QTest::newRow("unexpectedMessage") << GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer << tr("The program received an unexpected message from the server.");
			QTest::newRow("pinBlocked") << GlobalStatus::Code::Workflow_Pin_Blocked_And_Puk_Objectionable << tr("After three wrong entries your PIN is blocked. Please use the PIN management in this app to unblock it with the help of your PUK.");
			QTest::newRow("preverificationDevelopermode") << GlobalStatus::Code::Workflow_Preverification_Developermode_Error << tr("Using the developer mode is only allowed in a test environment.");
			QTest::newRow("noUniqueAtCvc") << GlobalStatus::Code::Workflow_No_Unique_AtCvc << tr("No unique AT CVC");
			QTest::newRow("noUniqueDvCvc") << GlobalStatus::Code::Workflow_No_Unique_DvCvc << tr("No unique DV CVC");
			QTest::newRow("noPermission") << GlobalStatus::Code::Workflow_No_Permission_Error << tr("Authentication failed.");
			QTest::newRow("certificateNoDescription") << GlobalStatus::Code::Workflow_Certificate_No_Description << tr("No certificate description available.");
			QTest::newRow("certificateNoUrl") << GlobalStatus::Code::Workflow_Certificate_No_Url_In_Description << tr("No subject url available in certificate description.");
			QTest::newRow("hashError") << GlobalStatus::Code::Workflow_Certificate_Hash_Error << tr("The certificate description does not match the certificate.");
			QTest::newRow("certificateSop") << GlobalStatus::Code::Workflow_Certificate_Sop_Error << tr("The subject URL in the certificate description and the TCToken URL don't satisfy the same origin policy.");
			QTest::newRow("wrongParameter") << GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation << tr("Application was invoked with wrong parameters.");
			QTest::newRow("readerCommunication") << GlobalStatus::Code::Workflow_Reader_Communication_Error << tr("An error occurred while communicating with the card reader.");
			QTest::newRow("incompleteInformation") << GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided << tr("The server provided no or incomplete information. Your personal data could not be read out.");
			QTest::newRow("deviceConnection") << GlobalStatus::Code::Workflow_Reader_Device_Connection_Error << tr("An error occurred while connecting to a reader device.");
			QTest::newRow("deviceScan") << GlobalStatus::Code::Workflow_Reader_Device_Scan_Error << tr("An error occurred while scanning for reader devices.");
			QTest::newRow("normalClose") << GlobalStatus::Code::RemoteReader_CloseCode_NormalClose << tr("The remote reader connection was closed properly.");
			QTest::newRow("abnormalClose") << GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose << tr("The remote card reader connection was not closed properly.");
			QTest::newRow("closeUndefined") << GlobalStatus::Code::RemoteReader_CloseCode_Undefined << tr("Undefined error code occured when the remote card reader connection was closed.");
			QTest::newRow("invalidRequest") << GlobalStatus::Code::RemoteConnector_InvalidRequest << tr("Remote reader connection request contains invalid parameters.");
			QTest::newRow("emptyPassword") << GlobalStatus::Code::RemoteConnector_EmptyPassword << tr("Empty password in extended encryption of remote reader connection request.");
			QTest::newRow("noSupportedApiLevel") << GlobalStatus::Code::RemoteConnector_NoSupportedApiLevel << tr("Your remote reader version is incompatible with the local version. Please install the latest AusweisApp2 version on both your smartphone and your computer.");
			QTest::newRow("connectionTimeout") << GlobalStatus::Code::RemoteConnector_ConnectionTimeout << tr("A timeout occurred while trying to establish a connection to a remote reader.");
			QTest::newRow("connectionError") << GlobalStatus::Code::RemoteConnector_ConnectionError << tr("An error occurred while trying to establish a connection to a remote reader.");
			QTest::newRow("hostRefused") << GlobalStatus::Code::RemoteConnector_RemoteHostRefusedConnection << tr("Remote device has rejected the connection. Please check the pairing code.");
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
