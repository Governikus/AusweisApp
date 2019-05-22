/*!
 * \brief Unit tests for \ref CVCertificateChain
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "asn1/CVCertificateChainBuilder.h"
#include "TestFileHelper.h"


using namespace governikus;


class test_CVCertificateChainBuilder
	: public QObject
{
	Q_OBJECT
	QSharedPointer<const CVCertificate> mCvca_DETESTeID00001;
	QSharedPointer<const CVCertificate> mCvca_DETESTeID00002_DETESTeID00001;
	QSharedPointer<const CVCertificate> mCvca_DETESTeID00002;
	QSharedPointer<const CVCertificate> mCvca_DETESTeID00004_DETESTeID00002;
	QSharedPointer<const CVCertificate> mCvca_DETESTeID00004;
	QSharedPointer<const CVCertificate> mCvdv_DEDVeIDDPST00035;
	QSharedPointer<const CVCertificate> mCvat_DEDEMODEV00038;

	QSharedPointer<const CVCertificate> mCvca_DETESTeID00005_DETESTeID00004;
	QSharedPointer<const CVCertificate> mCvca_DETESTeID00005;
	QSharedPointer<const CVCertificate> mCvdv_DEDVtIDGVNK00005;
	QSharedPointer<const CVCertificate> mCvat_DEDEVDEMO00020;


	QByteArray readFile(const QString& pFileName)
	{
		return TestFileHelper::readFile(QString(":/card/").append(pFileName));
	}


	/*!
	 * Creates an object of EstablishPaceChannelOutput with CARcurr, and CARprev equal to
	 */
	static EstablishPaceChannelOutput createPaceOutput(QSharedPointer<const CVCertificate> pCvcCarCurr, QSharedPointer<const CVCertificate> pCvcCarPrev)
	{
		EstablishPaceChannelOutput output;
		if (pCvcCarCurr != nullptr)
		{
			output.setCarCurr(pCvcCarCurr->getBody().getCertificateHolderReference());
		}
		if (pCvcCarPrev != nullptr)
		{
			output.setCarPrev(pCvcCarPrev->getBody().getCertificateHolderReference());
		}
		return output;
	}


	private Q_SLOTS:
		void initTestCase()
		{
			mCvca_DETESTeID00001 = CVCertificate::fromHex(readFile("cvca-DETESTeID00001.hex"));
			mCvca_DETESTeID00002_DETESTeID00001 = CVCertificate::fromHex(readFile("cvca-DETESTeID00002_DETESTeID00001.hex"));
			mCvca_DETESTeID00002 = CVCertificate::fromHex(readFile("cvca-DETESTeID00002.hex"));
			mCvca_DETESTeID00004_DETESTeID00002 = CVCertificate::fromHex(readFile("cvca-DETESTeID00004_DETESTeID00002.hex"));
			mCvca_DETESTeID00004 = CVCertificate::fromHex(readFile("cvca-DETESTeID00004.hex"));
			mCvdv_DEDVeIDDPST00035 = CVCertificate::fromHex(readFile("cvdv-DEDVeIDDPST00035.hex")); // DV issued from CVCA DETESTeID00004
			mCvat_DEDEMODEV00038 = CVCertificate::fromHex(readFile("cvat-DEDEMODEV00038.hex"));     // TA issued from DV DEDVeIDDPST00035

			mCvca_DETESTeID00005_DETESTeID00004 = CVCertificate::fromHex(readFile("cvca-DETESTeID00005_DETESTeID00004.hex"));
			mCvca_DETESTeID00005 = CVCertificate::fromHex(readFile("cvca-DETESTeID00005.hex"));
			mCvdv_DEDVtIDGVNK00005 = CVCertificate::fromHex(readFile("cvdv-DEDVtIDGVNK00005.hex")); // DV issued from CVCA DETESTeID00005
			mCvat_DEDEVDEMO00020 = CVCertificate::fromHex(readFile("cvat-DEDEVDEMO00020.hex"));     // TA issued from DV DEDVtIDGVNK00005
		}


		void productionAndTestEnvironment()
		{
			CVCertificateChainBuilder builderProduction(true);
			QVERIFY(builderProduction.getChainStartingWith(mCvca_DETESTeID00001).isProductive());
			QVERIFY(builderProduction.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001)).isProductive());

			CVCertificateChainBuilder builderTest(false);
			QVERIFY(!builderTest.getChainStartingWith(mCvca_DETESTeID00001).isProductive());
			QVERIFY(!builderTest.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001)).isProductive());
		}


		void emptyPool_getChainStartingWith()
		{
			CVCertificateChainBuilder builder;

			CVCertificateChain chain = builder.getChainStartingWith(mCvca_DETESTeID00001);

			QVERIFY(!chain.isValid());
		}


		void emptyPool_getChainForCertificationAuthority()
		{
			CVCertificateChainBuilder builder;

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001));

			QVERIFY(!chain.isValid());
		}


		void noDV_getChainStartingWith()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			// list.append(mCvdv_DEDVeIDDPST00035); do not include the DV certificate
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca_DETESTeID00001);

			QVERIFY(!chain.isValid());
		}


		void noDV_getChainForCertificationAuthority()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			// list.append(mCvdv_DEDVeIDDPST00035); do not include the DV certificate
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001));

			QVERIFY(!chain.isValid());
		}


		void noAT_getChainStartingWith()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			// list.append(mCvat_DEDEMODEV00038); do not include the AT certificate
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca_DETESTeID00001);

			QVERIFY(!chain.isValid());
		}


		void noAT_getChainForCertificationAuthority()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			// list.append(mCvat_DEDEMODEV00038); do not include the AT certificate
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001));

			QVERIFY(!chain.isValid());
		}


		void onlyATandDV_getChainStartingWith()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			// list.append(mCvca_DETESTeID00005);
			// list.append(mCvca_DETESTeID00005_DETESTeID00004); do not include any CVCA certificates
			// list.append(mCvca_DETESTeID00004);
			// list.append(mCvca_DETESTeID00004_DETESTeID00002);
			// list.append(mCvca_DETESTeID00002);
			// list.append(mCvca_DETESTeID00002_DETESTeID00001);
			// list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainStartingWith(mCvdv_DEDVeIDDPST00035);

			QVERIFY(chain.isValid());
		}


		void onlyATandDV_getChainForCertificationAuthority()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			// list.append(mCvca_DETESTeID00005);
			// list.append(mCvca_DETESTeID00005_DETESTeID00004); do not include any CVCA certificates
			// list.append(mCvca_DETESTeID00004);
			// list.append(mCvca_DETESTeID00004_DETESTeID00002);
			// list.append(mCvca_DETESTeID00002);
			// list.append(mCvca_DETESTeID00002_DETESTeID00001);
			// list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00004_DETESTeID00002, mCvca_DETESTeID00004_DETESTeID00002));

			QVERIFY(chain.isValid());
		}


		void missingLinkCert_getChainStartingWith()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			// list.append(mCvca_DETESTeID00002_DETESTeID00001); do not include link certificate
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca_DETESTeID00001);
			QVERIFY(!chain.isValid());

			chain = builder.getChainStartingWith(mCvca_DETESTeID00002);
			QVERIFY(chain.isValid());

			chain = builder.getChainStartingWith(mCvca_DETESTeID00004_DETESTeID00002);
			QVERIFY(chain.isValid());
		}


		void missingLinkCert_getChainForCertificationAuthority()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			// list.append(mCvca_DETESTeID00002_DETESTeID00001); do not include link certificate
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001));
			QVERIFY(!chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00002, mCvca_DETESTeID00002));
			QVERIFY(chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00004_DETESTeID00002, mCvca_DETESTeID00004_DETESTeID00002));
			QVERIFY(chain.isValid());
		}


		/*
		 * This covers the CVCA renewal scenario.
		 * The terminal certificate is issued under CVCA4 but a newer CVCA5
		 * is already published:
		 *
		 * CVCA4 -- LINK_CVCA4_CVCA5
		 *   |
		 *   |
		 * DV_old
		 *   |
		 *   |
		 * AT_old
		 */
		void match_getChainStartingWith()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca_DETESTeID00001);
			QVERIFY(chain.isValid());

			chain = builder.getChainStartingWith(mCvca_DETESTeID00002);
			QVERIFY(chain.isValid());

			chain = builder.getChainStartingWith(mCvca_DETESTeID00004_DETESTeID00002);
			QVERIFY(chain.isValid());
		}


		void match_getChainForCertificationAuthority()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00001, mCvca_DETESTeID00001));
			QVERIFY(chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00002, mCvca_DETESTeID00002));
			QVERIFY(chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00004_DETESTeID00002, mCvca_DETESTeID00004_DETESTeID00002));
			QVERIFY(chain.isValid());
		}


		void noMatch_getChainStartingWith()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca_DETESTeID00005);

			QVERIFY(!chain.isValid());
		}


		void noMatch_getChainForCertificationAuthority()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca_DETESTeID00005, mCvca_DETESTeID00005));

			QVERIFY(!chain.isValid());
		}


		void getChainForCertificationAuthority_forCarCurr()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);
			EstablishPaceChannelOutput output = createPaceOutput(mCvca_DETESTeID00004_DETESTeID00002, mCvca_DETESTeID00002);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(output);

			QVERIFY(chain.isValid());
			QCOMPARE(chain.at(0)->getBody().getCertificationAuthorityReference(), output.getCARcurr());
			QCOMPARE(chain.size(), 2);
		}


		void getChainForCertificationAuthority_forCarPrev()
		{
			QVector<QSharedPointer<const CVCertificate> > list;
			list.append(mCvca_DETESTeID00005);
			list.append(mCvca_DETESTeID00005_DETESTeID00004);
			list.append(mCvca_DETESTeID00004);
			list.append(mCvca_DETESTeID00004_DETESTeID00002);
			list.append(mCvca_DETESTeID00002);
			list.append(mCvca_DETESTeID00002_DETESTeID00001);
			list.append(mCvca_DETESTeID00001);
			list.append(mCvdv_DEDVeIDDPST00035);
			list.append(mCvat_DEDEMODEV00038);
			CVCertificateChainBuilder builder(list, false);
			EstablishPaceChannelOutput output = createPaceOutput(mCvca_DETESTeID00005, mCvca_DETESTeID00002);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(output);

			QVERIFY(chain.isValid());
			QCOMPARE(chain.at(0)->getBody().getCertificationAuthorityReference(), output.getCARprev());
			QCOMPARE(chain.size(), 3);
		}


};


QTEST_GUILESS_MAIN(test_CVCertificateChainBuilder)
#include "test_CVCertificateChainBuilder.moc"
