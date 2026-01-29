/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/SignatureChecker.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/CVCertificate.h"

#include "Converter.h"
#include "TestFileHelper.h"

#include <QtTest>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/obj_mac.h>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_SignatureChecker
	: public QObject
{
	Q_OBJECT

	QList<QSharedPointer<const CVCertificate>> cvcs;

	static QSharedPointer<const CVCertificate> load(const QString& pName)
	{
		QByteArray bytes = TestFileHelper::readFile(pName);
		return Converter::certificatefromHex(bytes);
	}

	private Q_SLOTS:
		void init()
		{
			cvcs.clear();
			cvcs.append(load(":/card/cvca-DETESTeID00001.hex"_L1));
			cvcs.append(load(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"_L1));
			cvcs.append(load(":/card/cvca-DETESTeID00004_DETESTeID00002.hex"_L1));
			cvcs.append(load(":/card/cvdv-DEDVeIDDPST00035.hex"_L1));
			cvcs.append(load(":/card/cvat-DEDEMODEV00038.hex"_L1));
			ERR_clear_error();
		}


		void cleanup()
		{
			QCOMPARE(getOpenSslError(), QByteArray());
		}


		void verifyEmptyList()
		{
			QList<QSharedPointer<const CVCertificate>> certs;
			SignatureChecker checker(certs);

			QTest::ignoreMessage(QtCriticalMsg, "Certificate chain is empty");
			QVERIFY(!checker.check());
		}


		void verifyNotSelfSigned()
		{
			QList<QSharedPointer<const CVCertificate>> certs(cvcs);
			certs.removeAt(0);
			SignatureChecker checker(certs);

			QTest::ignoreMessage(QtCriticalMsg, "Certificate verification failed: \"DETESTeID00002\"");
			QVERIFY(!checker.check());
#if OPENSSL_VERSION_NUMBER >= 0x30500010L
			QCOMPARE(getOpenSslError(), QByteArray("error:030000EA:digital envelope routines::provider signature failure"));
#endif
		}


		void verifyNoCertificateWithCurveParameters()
		{
			QList<QSharedPointer<const CVCertificate>> certs(cvcs);
			certs.removeAt(2);
			certs.removeAt(1);
			certs.removeAt(0);
			SignatureChecker checker(certs);

			QTest::ignoreMessage(QtCriticalMsg, "No elliptic curve parameters");
			QVERIFY(!checker.check());
		}


		void verifyNoValidChain()
		{
			QList<QSharedPointer<const CVCertificate>> certs(cvcs);
			certs.removeAt(2);
			SignatureChecker checker(certs);

			QTest::ignoreMessage(QtCriticalMsg, "Certificate verification failed: \"DEDVeIDDPST00035\"");
			QVERIFY(!checker.check());
#if OPENSSL_VERSION_NUMBER >= 0x30500010L
			QCOMPARE(getOpenSslError(), QByteArray("error:030000EA:digital envelope routines::provider signature failure"));
#endif
		}


		void verifyValidChain()
		{
			qDebug() << cvcs.size();
			SignatureChecker checker(cvcs);

			QVERIFY(checker.check());
		}


		void checkSignature_fail()
		{
			QTest::ignoreMessage(QtCriticalMsg, "Cannot fetch signing key");
			const auto& certificate = load(":/card/cvat-DEDEMODEV00038.hex"_L1);
			QVERIFY(!SignatureChecker::checkSignature(nullptr, certificate, &certificate->getBody().getPublicKey()));

			QTest::ignoreMessage(QtCriticalMsg, "Missing signing key");
			QVERIFY(!SignatureChecker::checkSignature(nullptr, QByteArray(), QByteArray(), QCryptographicHash::Algorithm::Sha256));

			QTest::ignoreMessage(QtCriticalMsg, "Cannot init verify ctx");
			QSharedPointer<EVP_PKEY> key(EVP_PKEY_new(), [](EVP_PKEY* pKey){EVP_PKEY_free(pKey);});
			QVERIFY(!SignatureChecker::checkSignature(key, QByteArray(), QByteArray(), QCryptographicHash::Algorithm::Sha256));
#if OPENSSL_VERSION_NUMBER < 0x30000000L
			QCOMPARE(getOpenSslError(), QByteArray("error:0609D09C:digital envelope routines:int_ctx_new:unsupported algorithm | error:0608F096:digital envelope routines:EVP_PKEY_verify_init:operation not supported for this keytype"));
#else
			QVERIFY(getOpenSslError().startsWith(QByteArray("error:0308010C:digital envelope routines::unsupported | error:03")));
#endif
		}


};

QTEST_GUILESS_MAIN(test_SignatureChecker)
#include "test_SignatureChecker.moc"
