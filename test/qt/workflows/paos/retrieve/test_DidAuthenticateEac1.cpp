/*!
 * \brief Unit tests for \DidAuthenticateEac1
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEac1.h"

#include "Env.h"
#include "LogHandler.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_DidAuthenticateEac1
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void parseXml()
		{
			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml");
			QScopedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
			QVERIFY(!eac1.isNull());

			QCOMPARE(eac1->getConnectionHandle().getCardApplication(), QString("4549445F49534F5F32343732375F42415345"));
			QCOMPARE(eac1->getConnectionHandle().getContextHandle(), QString("4549445F4946445F434F4E544558545F42415345"));
			QCOMPARE(eac1->getConnectionHandle().getIfdName(), QString("REINER SCT cyberJack RFID komfort USB 52"));
			QCOMPARE(eac1->getConnectionHandle().getSlotHandle(), QString("37343139303333612D616163352D343331352D386464392D656166393664636661653361"));
			QCOMPARE(eac1->getConnectionHandle().getSlotIndex(), QString("0"));

			QCOMPARE(eac1->getDidName(), QString("PIN"));

			QCOMPARE(eac1->getAuthenticatedAuxiliaryDataAsBinary(), QByteArray::fromHex("67447315060904007F000703010401530831393932313230367315060904007F000703010402530832303133313230367314060904007F000703010403530702760400110000"));
			QCOMPARE(eac1->getAuthenticatedAuxiliaryData()->getAgeVerificationDate(), QDate(1992, 12, 6));
			QCOMPARE(eac1->getCvCertificates().size(), 6);
			QCOMPARE(eac1->getCertificateDescriptionAsBinary(), QByteArray::fromHex("3082013B060A04007F00070301030101A1160C14476F7665726E696B757320546573742044564341A21A1318687474703A2F2F7777772E676F7665726E696B75732E6465A31A0C18476F7665726E696B757320476D6248202620436F2E204B47A420131E68747470733A2F2F746573742E676F7665726E696B75732D6569642E6465A581940C81914E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476F7665726E696B757320476D6248202620436F2E204B470D0A486F6368736368756C72696E6720340D0A3238333539204272656D656E0D0A452D4D61696C3A206B6F6E74616B7440676F7665726E696B75732E646509A72431220420CCB65AC1D48E9CD43876CA82CFE83C43D711294D4A40F68811ACB715AAA6C8AB"));
			QCOMPARE(eac1->getCertificateDescription()->getIssuerName(), QStringLiteral("Governikus Test DVCA"));
			QVERIFY(eac1->getOptionalChat());
			QVERIFY(eac1->getRequiredChat());
			QCOMPARE(eac1->getTransactionInfo(), QString("this is a test for TransactionInfo"));
		}


		// Test data from Test TS_TA_2.1.1 from TR-03105-5.2
		void test_TS_TA_2_1_1()
		{
			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1_TS_TA_2.1.1.xml");
			QScopedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));

			QVERIFY(eac1 == nullptr);
		}


		void checkUniqueEntries_data()
		{
			QTest::addColumn<QByteArray>("replaceIdentifier");
			QTest::addColumn<QByteArray>("replaceContent");
			QTest::addColumn<QString>("templateXml");

			const QByteArray placeHolder("<!-- PLACEHOLDER -->");
			const QString templ1(":/paos/DIDAuthenticateEAC1_template.xml");
			const QString templ2(":/paos/DIDAuthenticateEAC1_template2.xml");

			QTest::newRow("DIDName") << QByteArray("<!-- DIDNAME -->") << QByteArray("PIN") << templ1;
			QTest::newRow("CertificateDescription") << placeHolder << QByteArray("3082013b060a04007f00070301030101a1160c14476f7665726e696b757320546573742044564341a21a1318687474703a2f2f7777772e676f7665726e696b75732e6465a31a0c18476f7665726e696b757320476d6248202620436f2e204b47a420131e68747470733a2f2f746573742e676f7665726e696b75732d6569642e6465a581940c81914e616d652c20416e7363687269667420756e6420452d4d61696c2d4164726573736520646573204469656e737465616e626965746572733a0d0a476f7665726e696b757320476d6248202620436f2e204b470d0a486f6368736368756c72696e6720340d0a3238333539204272656d656e0d0a452d4d61696c3a206b6f6e74616b7440676f7665726e696b75732e646509a72431220420ccb65ac1d48e9cd43876ca82cfe83c43d711294d4a40f68811acb715aaa6c8ab") << templ1;
			QTest::newRow("RequiredCHAT") << placeHolder << QByteArray("7F4C12060904007F00070301020253050001137C05") << templ1;
			QTest::newRow("OptionalCHAT") << placeHolder << QByteArray("7F4C12060904007F00070301020253050004008302") << templ1;
			QTest::newRow("AuthenticatedAuxiliaryData") << placeHolder << QByteArray("67447315060904007F000703010401530831393932313230367315060904007F000703010402530832303133313230367314060904007F000703010403530702760400110000") << templ1;
			QTest::newRow("TransactionInfo") << placeHolder << QByteArray("this is a test for TransactionInfo") << templ1;

			QTest::newRow("ContextHandle") << QByteArray("<!-- CONTEXTHANDLE -->") << QByteArray("4549445F4946445F434F4E544558545F42415345") << templ2;
			QTest::newRow("IFDName") << QByteArray("<!-- IFDNAME -->") << QByteArray("REINER SCT cyberJack RFID komfort USB 52") << templ2;
			QTest::newRow("SlotIndex") << QByteArray("<!-- SLOTINDEX -->") << QByteArray("0") << templ2;
			QTest::newRow("CardApplication") << QByteArray("<!-- CARDAPPLICATION -->") << QByteArray("4549445F49534F5F32343732375F42415345") << templ2;
			QTest::newRow("SlotHandle") << QByteArray("<!-- SLOTHANDLE -->") << QByteArray("37343139303333612D616163352D343331352D386464392D656166393664636661653361") << templ2;
		}


		void checkUniqueEntries()
		{
			QFETCH(QByteArray, replaceIdentifier);
			QFETCH(QByteArray, replaceContent);
			QFETCH(QString, templateXml);

			const QByteArray tag = QByteArray(QTest::currentDataTag());
			const QByteArray data = '<' + tag + '>' + replaceContent + "</" + tag + '>';

			QByteArray content = TestFileHelper::readFile(templateXml);
			content = content.replace(replaceIdentifier, data + data);

			QScopedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
			QVERIFY(eac1.isNull());

			const QByteArray duplicateUniqueElement = "Duplicate unique element: \"" + tag + "\"";
			QVERIFY(Env::getSingleton<LogHandler>()->getBacklog().contains(duplicateUniqueElement));
		}


		void acceptedEidType_data()
		{
			QTest::addColumn<QByteArray>("replaceContent");
			QTest::addColumn<bool>("parsingSuccessful");
			QTest::addColumn<QList<AcceptedEidType>>("acceptedEidTypes");

			QTest::newRow("AcceptedEidType - Empty 1") << QByteArray() << true << QList<AcceptedEidType>({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED});
			QTest::newRow("AcceptedEidType - Empty 2") << QByteArray("<AcceptedEIDType></AcceptedEIDType>") << false << QList<AcceptedEidType>({});
			QTest::newRow("AcceptedEidType - CardCertified") << QByteArray("<AcceptedEIDType>CardCertified</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::CARD_CERTIFIED});
			QTest::newRow("AcceptedEidType - SECertified") << QByteArray("<AcceptedEIDType>SECertified</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::SE_CERTIFIED});
			QTest::newRow("AcceptedEidType - SEEndorsed") << QByteArray("<AcceptedEIDType>SEEndorsed</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::SE_ENDORSED});
			QTest::newRow("AcceptedEidType - HWKeyStore") << QByteArray("<AcceptedEIDType>HWKeyStore</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::HW_KEYSTORE});
			QTest::newRow("AcceptedEidType - CardCertified, SECertified") << QByteArray("<AcceptedEIDType>CardCertified</AcceptedEIDType><AcceptedEIDType>SECertified</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED});
			QTest::newRow("AcceptedEidType - CardCertified, SECertified, SEEndorsed") << QByteArray("<AcceptedEIDType>CardCertified</AcceptedEIDType><AcceptedEIDType>SECertified</AcceptedEIDType><AcceptedEIDType>SEEndorsed</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED});
			QTest::newRow("AcceptedEidType - CardCertified, SECertified, SEEndorsed, HWKeyStore") << QByteArray("<AcceptedEIDType>CardCertified</AcceptedEIDType><AcceptedEIDType>SECertified</AcceptedEIDType><AcceptedEIDType>SEEndorsed</AcceptedEIDType><AcceptedEIDType>HWKeyStore</AcceptedEIDType>") << true << QList<AcceptedEidType>({AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED, AcceptedEidType::HW_KEYSTORE});
			QTest::newRow("AcceptedEidType - Invalid 1") << QByteArray("<AcceptedEIDType>NotCertified</AcceptedEIDType>") << false << QList<AcceptedEidType>({});
			QTest::newRow("AcceptedEidType - Invalid 2") << QByteArray("<AcceptedEIDType>HelloWorld!") << false << QList<AcceptedEidType>({});
		}


		void acceptedEidType()
		{
			QFETCH(QByteArray, replaceContent);
			QFETCH(bool, parsingSuccessful);
			QFETCH(QList<AcceptedEidType>, acceptedEidTypes);

			QByteArray content = TestFileHelper::readFile(QString(":/paos/DIDAuthenticateEAC1_template.xml"));
			content = content.replace(QByteArray("<!-- PLACEHOLDER -->"), replaceContent);

			QScopedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));
			if (parsingSuccessful)
			{
				QVERIFY(!eac1.isNull());
				QCOMPARE(eac1->getAcceptedEidTypes().size(), acceptedEidTypes.size());
				if (acceptedEidTypes.isEmpty())
				{
					QVERIFY(eac1->getAcceptedEidTypes().isEmpty());
				}
				for (const auto& accepted : acceptedEidTypes)
				{
					QVERIFY(eac1->getAcceptedEidTypes().contains(accepted));
				}

			}
			else
			{
				QVERIFY(eac1.isNull());
			}
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEac1)
#include "test_DidAuthenticateEac1.moc"
