/*!
 * \brief Unit tests for \ref MsgHandlerCertificate
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "messages/MsgHandlerCertificate.h"

#include "InternalActivationContext.h"
#include "MessageDispatcher.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "TestAuthContext.h"
#include <QtTest>

using namespace governikus;

class test_MsgHandlerCertificate
	: public QObject
{
	Q_OBJECT

	QSharedPointer<AuthContext> getContext()
	{
		QSharedPointer<TestAuthContext> context(new TestAuthContext(new InternalActivationContext(QUrl("http://dummy")), ":/paos/DIDAuthenticateEAC1.xml"));
		context->setRequiredAccessRights({AccessRight::READ_DG01});
		context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION});
		return context;
	}


	private Q_SLOTS:
		void getCertificateBadState()
		{
			MessageDispatcher dispatcher;

			QByteArray msg = QByteArray("{\"cmd\": \"GET_CERTIFICATE\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"GET_CERTIFICATE\",\"msg\":\"BAD_STATE\"}"));

			dispatcher.init(getContext());
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"GET_CERTIFICATE\",\"msg\":\"BAD_STATE\"}"));
		}


		void getCertificate()
		{
			auto context = getContext();

			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());
			QByteArray msg = "{\"cmd\": \"GET_CERTIFICATE\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"description\":{\"issuerName\":\"Deutsche Post Com, Gesch\xC3\xA4" "ftsfeld Signtrust\",\"issuerUrl\":\"http://www.signtrust.de\",\"purpose\":\"Demonstration des eID-Service\",\"subjectName\":\"bos KG\",\"subjectUrl\":\"https://dev-demo.governikus-eid.de:8443\",\"termsOfUsage\":\"Anschrift:\\r\\nbremen online services GmbH & Co. KG\\r\\nAm Fallturm 9\\r\\n28359 Bremen\\r\\n\\r\\nE-Mail-Adresse:\\r\\nhb@bos-bremen.de\\r\\n\\r\\nZweck des Auslesevorgangs:\\r\\nDemonstration des eID-Service\\r\\n\\r\\nZust\xC3\xA4ndige Datenschutzaufsicht:\\r\\nDie Landesbeauftragte f\xC3\xBCr Datenschutz und Informationsfreiheit der Freien Hansestadt Bremen\\r\\nArndtstra\xC3\x9F" "e 1\\r\\n27570 Bremerhaven\"},\"msg\":\"CERTIFICATE\",\"validity\":{\"effectiveDate\":\"2013-11-27\",\"expirationDate\":\"2013-12-26\"}}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerCertificate)
#include "test_MsgHandlerCertificate.moc"
