/*!
 * \brief Unit tests for \ref CertificateDescriptionModel
 *
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateDescriptionModel.h"

#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


class test_CertificateDescriptionModel
	: public QObject
{
	Q_OBJECT

	private:
		CertificateDescriptionModel* mModel = nullptr;
		QSharedPointer<AuthContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			mModel = Env::getSingleton<CertificateDescriptionModel>();
		}


		void cleanup()
		{
			mModel->resetContext();
			mContext.clear();
		}


		void test_ResetContext()
		{
			QSignalSpy spy(mModel, &CertificateDescriptionModel::fireChanged);
			mModel->resetContext(mContext);
			QCOMPARE(mModel->rowCount(), 4);

			const QString termsOfUsage = QStringLiteral("Name, Anschrift und E-Mail-Adresse des Diensteanbieters:\nGovernikus GmbH & Co. KG\nHochschulring 4\n28359 Bremen\nE-Mail: kontakt@governikus.de ");
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::LABEL), QString("Provider"));
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Governikus GmbH & Co. KG\nhttps://test.governikus-eid.de"));
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::LABEL), QString("Certificate issuer"));
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Governikus Test DVCA\nhttp://www.governikus.de"));
			QCOMPARE(mModel->data(mModel->index(2), CertificateDescriptionModel::UserRoles::LABEL), QString("Provider information"));
			QCOMPARE(mModel->data(mModel->index(2), CertificateDescriptionModel::UserRoles::TEXT), termsOfUsage);
			QCOMPARE(mModel->data(mModel->index(3), CertificateDescriptionModel::UserRoles::LABEL), QString("Validity"));
			QCOMPARE(spy.count(), 1);
		}


		void test_Data()
		{
			mModel->resetContext(mContext);

			QCOMPARE(mModel->data(QModelIndex(), CertificateDescriptionModel::UserRoles::LABEL), QVariant());

			const auto& rowTooLarge = mModel->index(7, 0);
			QCOMPARE(mModel->data(rowTooLarge, CertificateDescriptionModel::UserRoles::LABEL), QVariant());

			const auto& validModel = mModel->index(1, 0);

			QCOMPARE(mModel->data(validModel, CertificateDescriptionModel::UserRoles::LABEL), QString("Certificate issuer"));
			QCOMPARE(mModel->data(validModel, CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Governikus Test DVCA\nhttp://www.governikus.de"));
			QCOMPARE(mModel->data(validModel, 0), QVariant());
			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::LABEL), QString("Validity"));
			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::TEXT), QString("5/21/20 - 6/20/20"));
		}


		void test_Validity()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1_ordered_certificates.xml"));
			mContext->initAccessRightManager(mContext->getDidAuthenticateEac1()->getCvCertificates().last());
			mModel->resetContext(mContext);

			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::LABEL), QString("Validity"));
			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::TEXT), QString("5/21/20 - 6/20/20"));
		}


		void test_Translation()
		{
			QSignalSpy spy(mModel, &CertificateDescriptionModel::fireChanged);
			mModel->onTranslationChanged();
			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_CertificateDescriptionModel)
#include "test_CertificateDescriptionModel.moc"
