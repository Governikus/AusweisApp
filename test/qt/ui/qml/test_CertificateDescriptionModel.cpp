/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateDescriptionModel.h"

#include "MockIfdServer.h"
#include "TestAuthContext.h"
#include "context/IfdServiceContext.h"

#include <QDebug>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_CertificateDescriptionModel
	: public QObject
{
	Q_OBJECT

	private:
		CertificateDescriptionModel* mModel = nullptr;
		QSharedPointer<AuthContext> mContext;

		static EstablishPaceChannel createDataToParse(const PacePasswordId& pinId)
		{
			const QByteArray chat = QByteArray::fromHex("7F4C12060904007F00070301020253050000000F0F");
			const QByteArray certDescription = QByteArray::fromHex("30 8202A4"
																   "        06 0A 04007F00070301030103"
																   "        A1 0E 0C0C442D547275737420476D6248"
																   "        A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
																   "        A5 820248"
																   "            04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978");

			return EstablishPaceChannel(pinId, chat, certDescription);
		}

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));
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
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::LABEL), "Provider"_L1);
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Governikus GmbH & Co. KG\nhttps://test.governikus-eid.de"));
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::LABEL), "Certificate issuer"_L1);
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Governikus Test DVCA\nhttp://www.governikus.de"));
			QCOMPARE(mModel->data(mModel->index(2), CertificateDescriptionModel::UserRoles::LABEL), "Provider information"_L1);
			QCOMPARE(mModel->data(mModel->index(2), CertificateDescriptionModel::UserRoles::TEXT), termsOfUsage);
			QCOMPARE(mModel->data(mModel->index(3), CertificateDescriptionModel::UserRoles::LABEL), "Validity"_L1);
			QCOMPARE(spy.count(), 1);
		}


		void test_Data()
		{
			mModel->resetContext(mContext);

			QCOMPARE(mModel->data(QModelIndex(), CertificateDescriptionModel::UserRoles::LABEL), QVariant());

			const auto& rowTooLarge = mModel->index(7, 0);
			QCOMPARE(mModel->data(rowTooLarge, CertificateDescriptionModel::UserRoles::LABEL), QVariant());

			const auto& validModel = mModel->index(1, 0);

			QCOMPARE(mModel->data(validModel, CertificateDescriptionModel::UserRoles::LABEL), "Certificate issuer"_L1);
			QCOMPARE(mModel->data(validModel, CertificateDescriptionModel::UserRoles::TEXT), QStringLiteral("Governikus Test DVCA\nhttp://www.governikus.de"));
			QCOMPARE(mModel->data(validModel, 0), QVariant());
			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::LABEL), "Validity"_L1);
			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::TEXT), "5/21/20 - 6/20/20"_L1);
		}


		void test_Validity()
		{
			mContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1_ordered_certificates.xml"_L1));
			mModel->resetContext(mContext);

			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::LABEL), "Validity"_L1);
			QCOMPARE(mModel->data(mModel->index(3, 0), CertificateDescriptionModel::UserRoles::TEXT), "5/21/20 - 6/20/20"_L1);
		}


		void test_Translation()
		{
			QSignalSpy spy(mModel, &CertificateDescriptionModel::fireChanged);
			mModel->onTranslationChanged();
			QCOMPARE(spy.count(), 1);
		}


		void test_IfdContext()
		{
			auto ifdContext = QSharedPointer<IfdServiceContext>::create(QSharedPointer<MockIfdServer>::create());
			mModel->resetContext(ifdContext);
			const auto& establishPaceChannel = createDataToParse(PacePasswordId::PACE_PIN);
			ifdContext->setEstablishPaceChannel(QSharedPointer<IfdEstablishPaceChannel>::create(QString(), establishPaceChannel, 6));

			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::LABEL), "Provider"_L1);
			QCOMPARE(mModel->data(mModel->index(0), CertificateDescriptionModel::UserRoles::TEXT), "Gesamtverband der deutschen Versicherungswirtschaft e.V.\n"_L1);
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::LABEL), "Certificate issuer"_L1);
			QCOMPARE(mModel->data(mModel->index(1), CertificateDescriptionModel::UserRoles::TEXT), "D-Trust GmbH\n"_L1);
			QCOMPARE(mModel->data(mModel->index(2), CertificateDescriptionModel::UserRoles::LABEL), "Provider information"_L1);
		}


};

QTEST_GUILESS_MAIN(test_CertificateDescriptionModel)
#include "test_CertificateDescriptionModel.moc"
