/*!
 * \brief Unit tests for \ref CertificateChecker
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateChecker.h"

#include "context/AuthContext.h"
#include "SecureStorage.h"

#include "MockActivationContext.h"

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
			certs = SecureStorage::getInstance().getUpdateCertificates();
			QVERIFY(certs.size() > 0);
		}


		void validUpdateCert()
		{
			const QSharedPointer<AuthContext> model(new AuthContext(QSharedPointer<MockActivationContext>::create()));
			std::function<void(const QUrl&, const QSslCertificate&)> saveCertificateFunc = [&model]
						(const QUrl& pUrl, const QSslCertificate& pCert)
					{
						model->addCertificateData(pUrl, pCert);
					};

			QCOMPARE(model->getCertificateList().size(), 0);
			QCOMPARE(CertificateChecker::checkAndSaveCertificate(certs.at(0), QUrl("dummy"), model->getDidAuthenticateEac1(), model->getDvCvc(), saveCertificateFunc), CertificateChecker::CertificateStatus::Good);
			QCOMPARE(model->getCertificateList().size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_CertificateChecker)
#include "test_CertificateChecker.moc"
