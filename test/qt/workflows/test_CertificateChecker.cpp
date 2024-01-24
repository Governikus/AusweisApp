/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref CertificateChecker
 */

#include "CertificateChecker.h"

#include "ResourceLoader.h"
#include "SecureStorage.h"
#include "context/AuthContext.h"

#include <QtTest>

using namespace governikus;

class test_CertificateChecker
	: public QObject
{
	Q_OBJECT

	QVector<QSslCertificate> certs;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			certs = Env::getSingleton<SecureStorage>()->getUpdateCertificates();
			QVERIFY(certs.size() > 0);
		}


		void validUpdateCert()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());

			QCOMPARE(context->getCertificateList().size(), 0);
			QCOMPARE(CertificateChecker::checkAndSaveCertificate(certs.at(0), QUrl("dummy"), context), CertificateChecker::CertificateStatus::Good);
			QCOMPARE(context->getCertificateList().size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_CertificateChecker)
#include "test_CertificateChecker.moc"
