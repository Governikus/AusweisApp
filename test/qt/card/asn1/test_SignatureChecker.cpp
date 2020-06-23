/*!
 * \brief Unit tests for \ref  SignatureChecker
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/obj_mac.h>

#include "asn1/CVCertificate.h"
#include "asn1/SignatureChecker.h"
#include "TestFileHelper.h"
#include <QDebug>

#include "pace/ec/EcUtil.h"


using namespace governikus;


class test_SignatureChecker
	: public QObject
{
	Q_OBJECT

	QVector<QSharedPointer<const CVCertificate> > cvcs;

	static QSharedPointer<const CVCertificate> load(const QString& pName)
	{
		QByteArray bytes = TestFileHelper::readFile(pName);
		return CVCertificate::fromHex(bytes);
	}

	private Q_SLOTS:
		void init()
		{
			cvcs.clear();
			cvcs.append(load(":/card/cvca-DETESTeID00001.hex"));
			cvcs.append(load(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"));
			cvcs.append(load(":/card/cvca-DETESTeID00004_DETESTeID00002.hex"));
			cvcs.append(load(":/card/cvdv-DEDVeIDDPST00035.hex"));
			cvcs.append(load(":/card/cvat-DEDEMODEV00038.hex"));
			ERR_clear_error();
		}


		void cleanup()
		{
			QCOMPARE(ERR_get_error(), 0);
		}


		void verifyEmptyList()
		{
			QVector<QSharedPointer<const CVCertificate> > certs;
			SignatureChecker checker(certs);

			QVERIFY(!checker.check());
		}


		void verifyNotSelfSigned()
		{
			QVector<QSharedPointer<const CVCertificate> > certs(cvcs);
			certs.removeAt(0);
			SignatureChecker checker(certs);

			QVERIFY(!checker.check());
		}


		void verifyNoCertificateWithCurveParameters()
		{
			QVector<QSharedPointer<const CVCertificate> > certs(cvcs);
			certs.removeAt(2);
			certs.removeAt(1);
			certs.removeAt(0);
			SignatureChecker checker(certs);

			QVERIFY(!checker.check());
		}


		void verifyNoValidChain()
		{
			QVector<QSharedPointer<const CVCertificate> > certs(cvcs);
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
