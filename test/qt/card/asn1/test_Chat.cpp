/*!
 * \brief Unit tests for \ref CHAT
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"
#include "asn1/KnownOIDs.h"

#include "asn1/Chat.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;

class test_CHAT
	: public QObject
{
	Q_OBJECT

	QByteArray decode_encode(const QByteArray& pHexEncodedChat)
	{
		QSharedPointer<CHAT> chat = CHAT::fromHex(pHexEncodedChat);
		return chat->encode().toHex().toUpper();
	}


	private Q_SLOTS:
		void createEmpty()
		{
			auto chat = newObject<CHAT>();
			QCOMPARE(chat->getAccessRole(), AccessRole::UNKNOWN);
			QCOMPARE(chat->getAccessRights().size(), 0);
			QCOMPARE(chat->getType(), QByteArray());
			QCOMPARE(chat->getTemplate(), QByteArray());
		}


		void setAccessRolegetAccessRole()
		{
			auto chat = newObject<CHAT>();
			chat->setType("0.4.0.127.0.1.2.3.4.5");

			chat->setAccessRole(AccessRole::CVCA);

			QCOMPARE(chat->getAccessRole(), AccessRole::CVCA);
			QVERIFY(chat->encode().toHex().toUpper().endsWith(QByteArray("5305C000000000")));
		}


		void setAccessRight()
		{
			auto chat = newObject<CHAT>();
			chat->setAccessRole(AccessRole::AT);
			chat->setType("0.4.0.127.0.1.2.3.4.5");

			chat->setAccessRight(AccessRight::AGE_VERIFICATION);
			QCOMPARE(chat->getTemplate().toHex().toUpper(), QByteArray("0000000001"));
			QVERIFY(chat->encode().toHex().toUpper().endsWith(QByteArray("0000000001")));
		}


		void setAccessRights()
		{
			auto chat = newObject<CHAT>();
			chat->setAccessRole(AccessRole::AT);
			chat->setType("0.4.0.127.0.1.2.3.4.5");

			QSet<AccessRight> rights;
			rights += AccessRight::PRIVILEGED_TERMINAL;
			rights += AccessRight::RESTRICTED_IDENTIFICATION;
			rights += AccessRight::COMMUNITY_ID_VERIFICATION;
			rights += AccessRight::AGE_VERIFICATION;
			rights += AccessRight::READ_DG01;
			rights += AccessRight::READ_DG02;
			rights += AccessRight::READ_DG03;
			rights += AccessRight::READ_DG04;
			chat->setAccessRights(rights);

			QCOMPARE(chat->getTemplate().toHex().toUpper(), QByteArray("0000000F0F"));
			QVERIFY(chat->encode().toHex().toUpper().endsWith(QByteArray("0000000F0F")));
		}


		void getAccessRights()
		{
			auto chat = CHAT::fromHex("7F4C12060904007F00070301020253050000000F0F");

			QSet<AccessRight> rights;
			rights += AccessRight::PRIVILEGED_TERMINAL;
			rights += AccessRight::RESTRICTED_IDENTIFICATION;
			rights += AccessRight::COMMUNITY_ID_VERIFICATION;
			rights += AccessRight::AGE_VERIFICATION;
			rights += AccessRight::READ_DG01;
			rights += AccessRight::READ_DG02;
			rights += AccessRight::READ_DG03;
			rights += AccessRight::READ_DG04;

			QCOMPARE(chat->getAccessRights(), rights);
		}


		void hasAccessRight()
		{
			auto chat = CHAT::fromHex("7F4C12060904007F00070301020253050000000001");

			QVERIFY(chat->hasAccessRight(AccessRight::AGE_VERIFICATION));
			QVERIFY(!chat->hasAccessRight(AccessRight::READ_DG01));
		}


		void removeAllAccessRights()
		{
			auto chat = CHAT::fromHex("7F4C12060904007F0007030102025305000FFFFFFF");

			chat->removeAllAccessRights();

			QVERIFY(chat->getAccessRights().isEmpty());
			QVERIFY(chat->encode().toHex().toUpper().endsWith(QByteArray("0000000000")));
		}


		void removeAccessRight()
		{
			auto chat = CHAT::fromHex("7F4C12060904007F00070301020253050000000001");

			chat->removeAccessRight(AccessRight::AGE_VERIFICATION);

			QVERIFY(!chat->hasAccessRight(AccessRight::AGE_VERIFICATION));
			QVERIFY(chat->encode().toHex().toUpper().endsWith(QByteArray("0000000000")));
		}


		void decode_encode_atCHAT()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C12060904007F000703010202").append("5305000513FF87");
			QCOMPARE(decode_encode(hexEncodedChat), hexEncodedChat);
		}


		void decode_encode_dvCHAT()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C12060904007F000703010202").append("5305400513FF87");
			QCOMPARE(decode_encode(hexEncodedChat), hexEncodedChat);
		}


		void decode_encode_cvcaCHAT()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C12060904007F000703010202").append("5305FC0F13FFFF");
			QCOMPARE(decode_encode(hexEncodedChat), hexEncodedChat);
		}


		void testMissingType()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C07").append("5305FC0F13FFFF");
			auto chat = CHAT::fromHex(hexEncodedChat);

			QVERIFY(chat == nullptr);
		}


		void testMissingTemplate()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C0B060904007F000703010202");
			auto chat = CHAT::fromHex(hexEncodedChat);

			QVERIFY(chat == nullptr);
		}


		void testTemplateWithWrongSize()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C11060904007F000703010202").append("5304FC0F13FF");
			auto chat = CHAT::fromHex(hexEncodedChat);

			QVERIFY(chat == nullptr);
		}


		void testUnsupprtedType()
		{
			// see test case TS_TA_2.1.1 from TR-03105-5.2
			QByteArray hexEncodedChat = QByteArray("7F4C12060904007F000703010201").append("5305FC0F13FFFF");
			auto chat = CHAT::fromHex(hexEncodedChat);

			QVERIFY(chat == nullptr);
		}


		void test()
		{
			QByteArray hexEncodedChat = QByteArray("7F4C12060904007F000703010202").append("5305FC0F13FFFF");
			auto chat = CHAT::fromHex(hexEncodedChat);

			QVERIFY(chat != nullptr);
			QCOMPARE(chat->getType(), QByteArray("0.4.0.127.0.7.3.1.2.2"));
			QCOMPARE(chat->getTemplate().toHex().toUpper(), QByteArray("FC0F13FFFF"));
			QVERIFY(chat->encode().toHex().toUpper().endsWith(QByteArray("FC0F13FFFF")));
		}


		void testDeepCopy()
		{
			QByteArray hexEncodedChat("7F4C12060904007F0007030102025305FC0F13FFFF");
			auto chat = CHAT::fromHex(hexEncodedChat);

			QSharedPointer<CHAT> copiedChat(new CHAT(*chat.data()));
			copiedChat->removeAllAccessRights();

			QCOMPARE(chat->encode().toHex().toUpper(), hexEncodedChat);
		}


};

QTEST_GUILESS_MAIN(test_CHAT)
#include "test_Chat.moc"
