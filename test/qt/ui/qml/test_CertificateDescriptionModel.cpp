/*!
 * \brief Unit tests for \ref CertificateDescriptionModel
 *
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
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
	QSharedPointer<CertificateDescriptionModel> mModel;
	QSharedPointer<AuthContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			mModel.reset(new CertificateDescriptionModel());
		}


		void cleanup()
		{
			mModel.clear();
			mContext.clear();
		}


		void test_ResetContext()
		{
			QSignalSpy spy(mModel.data(), &CertificateDescriptionModel::fireChanged);
			mModel->resetContext(mContext);
			QCOMPARE(mModel->rowCount(), 4);

			const QString termsOfUsage = QStringLiteral("Anschrift:\nbremen online services GmbH & Co. KG\nAm Fallturm 9\n28359 Bremen\n\nE-Mail-Adresse:\nhb@bos-bremen.de\n\nZweck des Auslesevorgangs:\nDemonstration des eID-Service\n\nZust\u00E4ndige Datenschutzaufsicht:\nDie Landesbeauftragte f\u00FCr Datenschutz und Informationsfreiheit der Freien Hansestadt Bremen\nArndtstra\u00DFe 1\n27570 Bremerhaven");
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::LABEL), QString("Provider"));
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("bos KG\nhttps://dev-demo.governikus-eid.de:8443"));
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::LABEL), QString("Certificate issuer"));
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Deutsche Post Com, Gesch\u00E4ftsfeld Signtrust\nhttp://www.signtrust.de"));
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
			QCOMPARE(mModel->data(validModel, CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Deutsche Post Com, Gesch\u00E4ftsfeld Signtrust\nhttp://www.signtrust.de"));
			QCOMPARE(mModel->data(validModel, 0), QVariant());
		}


};

QTEST_GUILESS_MAIN(test_CertificateDescriptionModel)
#include "test_CertificateDescriptionModel.moc"
