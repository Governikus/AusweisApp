/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref  SignatureChecker
 */

#include "asn1/SignatureChecker.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/CVCertificate.h"

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
		return CVCertificate::fromHex(bytes);
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

			QVERIFY(!checker.check());
		}


		void verifyNotSelfSigned()
		{
			QList<QSharedPointer<const CVCertificate>> certs(cvcs);
			certs.removeAt(0);
			SignatureChecker checker(certs);

			QVERIFY(!checker.check());
		}


		void verifyNoCertificateWithCurveParameters()
		{
			QList<QSharedPointer<const CVCertificate>> certs(cvcs);
			certs.removeAt(2);
			certs.removeAt(1);
			certs.removeAt(0);
			SignatureChecker checker(certs);

			QVERIFY(!checker.check());
		}


		void verifyNoValidChain()
		{
			QList<QSharedPointer<const CVCertificate>> certs(cvcs);
			certs.removeAt(2);
			SignatureChecker checker(certs);

			QVERIFY(!checker.check());
		}


		void verifyValidChain()
		{
			qDebug() << cvcs.size();
			SignatureChecker checker(cvcs);

			QVERIFY(checker.check());
		}


};

QTEST_GUILESS_MAIN(test_SignatureChecker)
#include "test_SignatureChecker.moc"
