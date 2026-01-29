/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/CVCertificateChainBuilder.h"

#include "Converter.h"
#include "TestFileHelper.h"

#include "ResourceLoader.h"
#include "SecureStorage.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

namespace CVCTemplate
{

// Helper function to create a certificate from a hexadecimal-encoded text template file
// to easily create chains by overwriting the existing names of the holder and authority.
// The template files are based on real certificates and differ in the length of the names
// of the holders and authorities.
static QSharedPointer<const CVCertificate> createCVCertificate(
		const QString& pFileName,
		const QString& pOldHolder,
		const QString& pOldAuthority,
		const QString& pHolder,
		const QString& pAuthority)
{
	Q_ASSERT(pHolder.size() <= pOldHolder.size());
	Q_ASSERT(pAuthority.size() <= pOldAuthority.size());

	QString newHolder = pHolder.leftJustified(pOldHolder.size(), u' ');
	QByteArray newHolderHex = newHolder.toLatin1().toHex();

	QString newAuthority = pAuthority.leftJustified(pOldAuthority.size(), u' ');
	QByteArray newAuthorityHex = newAuthority.toLatin1().toHex();

	QByteArray oldAuthorityHex = pOldAuthority.toLatin1().toHex();
	QByteArray oldHolderHex = pOldHolder.toLatin1().toHex();

	QByteArray hexBytes = TestFileHelper::readFile(QStringLiteral(":/card/").append(pFileName));
	hexBytes.replace(oldHolderHex, newHolderHex);
	hexBytes.replace(oldAuthorityHex, newAuthorityHex);
	QSharedPointer<const CVCertificate> cvc = Converter::certificatefromHex(hexBytes);

	Q_ASSERT(cvc->getBody().getCertificateHolderReference() == newHolder.toLatin1());
	Q_ASSERT(cvc->getBody().getCertificationAuthorityReference() == newAuthority.toLatin1());

	return cvc;
}


static QSharedPointer<const CVCertificate> fromCvca(const QString& pHolder, const QString& pAuthority)
{
	const auto templateFileName = "cvca-template-HolderXXXXXXXX_AuthorityXXXXX.hex"_L1;
	const auto oldHolder = QString("HolderXXXXXXXX"_L1);
	const auto oldAuthority = QString("AuthorityXXXXX"_L1);
	return createCVCertificate(templateFileName, oldHolder, oldAuthority, pHolder, pAuthority);
}


static QSharedPointer<const CVCertificate> fromAt(const QString& pHolder, const QString& pAuthority)
{
	const auto templateFileName = "cvat-template-HolderXXXXXXXX_AuthorityXXXXXXX.hex"_L1;
	const auto oldHolder = QString("HolderXXXXXXXX"_L1);
	const auto oldAuthority = QString("AuthorityXXXXXXX"_L1);
	return createCVCertificate(templateFileName, oldHolder, oldAuthority, pHolder, pAuthority);
}


static QSharedPointer<const CVCertificate> fromDv(const QString& pHolder, const QString& pAuthority)
{
	const auto templateFileName = "cvdv-template-HolderXXXXXXXXXX_AuthorityXXXXX.hex"_L1;
	const auto oldHolder = QString("HolderXXXXXXXXXX"_L1);
	const auto oldAuthority = QString("AuthorityXXXXX"_L1);
	return createCVCertificate(templateFileName, oldHolder, oldAuthority, pHolder, pAuthority);
}


} // namespace CVCTemplate


class test_CVCertificateChainBuilder
	: public QObject
{
	Q_OBJECT

	QSharedPointer<const CVCertificate> mCvca1_cvca1;
	QSharedPointer<const CVCertificate> mCvca2_cvca2;
	QSharedPointer<const CVCertificate> mCvca2_cvca1;
	QSharedPointer<const CVCertificate> mCvca4_cvca4;
	QSharedPointer<const CVCertificate> mCvca4_cvca2;
	QSharedPointer<const CVCertificate> mCvca5_cvca5;
	QSharedPointer<const CVCertificate> mCvca5_cvca4;

	QSharedPointer<const CVCertificate> mDv35_cvca4;

	QSharedPointer<const CVCertificate> mAt38_dv35;
	QSharedPointer<const CVCertificate> mAt20_dv5;

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
			mCvca1_cvca1 = CVCTemplate::fromCvca("cvca1"_L1, "cvca1"_L1);
			mCvca2_cvca2 = CVCTemplate::fromCvca("cvca2"_L1, "cvca2"_L1);
			mCvca2_cvca1 = CVCTemplate::fromCvca("cvca2"_L1, "cvca1"_L1);
			mCvca4_cvca4 = CVCTemplate::fromCvca("cvca4"_L1, "cvca4"_L1);
			mCvca4_cvca2 = CVCTemplate::fromCvca("cvca4"_L1, "cvca2"_L1);
			mCvca5_cvca5 = CVCTemplate::fromCvca("cvca5"_L1, "cvca5"_L1);
			mCvca5_cvca4 = CVCTemplate::fromCvca("cvca5"_L1, "cvca4"_L1);

			mDv35_cvca4 = CVCTemplate::fromDv("dv35"_L1, "cvca4"_L1);

			mAt38_dv35 = CVCTemplate::fromAt("at38"_L1, "dv35"_L1);
			mAt20_dv5 = CVCTemplate::fromAt("at20"_L1, "dv5"_L1);
		}


		void buildChain_productionEnvironment()
		{
			ResourceLoader::getInstance().init();
			const auto* secureStorage = Env::getSingleton<SecureStorage>();

			const auto prodLastCvcv = QByteArray("DECVCAeID00107");
			const auto prodRootCvcv = QByteArray("DECVCAeID00102"); // self signed

			QList<QSharedPointer<const CVCertificate>> all;
			all << CVCertificate::fromRaw(secureStorage->getCVRootCertificates(true));
			all << CVCertificate::fromRaw(secureStorage->getCVRootCertificates(false));
			all << CVCTemplate::fromDv("dv"_L1, QString::fromLatin1(prodLastCvcv));
			all << CVCTemplate::fromAt("at"_L1, "dv"_L1);

			CVCertificateChainBuilder builder(all);
			const auto chain = builder.getChainForCertificationAuthority(prodRootCvcv);
			QVERIFY(chain.isValid());
			QCOMPARE(chain.isProductive(), true);
		}


		void buildChain_testEnvironment()
		{
			ResourceLoader::getInstance().init();
			const auto* secureStorage = Env::getSingleton<SecureStorage>();

			const auto testLastCvcv = QByteArray("DETESTeID00008");
			const auto testRootCvcv = QByteArray("DETESTeID00001"); // self signed

			QList<QSharedPointer<const CVCertificate>> all;
			all << CVCertificate::fromRaw(secureStorage->getCVRootCertificates(true));
			all << CVCertificate::fromRaw(secureStorage->getCVRootCertificates(false));
			all << CVCTemplate::fromDv("dv"_L1, QString::fromLatin1(testLastCvcv));
			all << CVCTemplate::fromAt("at"_L1, "dv"_L1);

			CVCertificateChainBuilder builder(all);
			const auto chain = builder.getChainForCertificationAuthority(testRootCvcv);
			QVERIFY(chain.isValid());
			QCOMPARE(chain.isProductive(), false);
		}


		void emptyPool_getChainStartingWith()
		{
			CVCertificateChainBuilder builder;

			CVCertificateChain chain = builder.getChainStartingWith(mCvca1_cvca1);

			QVERIFY(!chain.isValid());
		}


		void emptyPool_getChainForCertificationAuthority()
		{
			CVCertificateChainBuilder builder;

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca1_cvca1, mCvca1_cvca1));

			QVERIFY(!chain.isValid());
		}


		void emptyList_getChainStartingWith()
		{
			QList<QSharedPointer<const CVCertificate>> emptyList;
			CVCertificateChainBuilder builder(emptyList);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca1_cvca1);

			QVERIFY(!chain.isValid());
		}


		void emptyList_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> emptyList;
			CVCertificateChainBuilder builder(emptyList);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca1_cvca1, mCvca1_cvca1));

			QVERIFY(!chain.isValid());
		}


		void noDV_getChainStartingWith()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			// list << mDv35_cvca4; do not include the DV certificate
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca1_cvca1);

			QVERIFY(!chain.isValid());
		}


		void noDV_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			// list << mDv35_cvca4; do not include the DV certificate
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca1_cvca1, mCvca1_cvca1));

			QVERIFY(!chain.isValid());
		}


		void noAT_getChainStartingWith()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			// list << mAt38_dv35; do not include the AT certificate
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca1_cvca1);

			QVERIFY(!chain.isValid());
		}


		void noAT_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			// list << mAt38_dv35; do not include the AT certificate
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca1_cvca1, mCvca1_cvca1));

			QVERIFY(!chain.isValid());
		}


		void onlyATandDV_getChainStartingWith()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			// list << mCvca5_cvca5;
			// list << mCvca5_cvca4; do not include any CVCA certificates
			// list << mCvca4_cvca4;
			// list << mCvca4_cvca2;
			// list << mCvca2_cvca2;
			// list << mCvca2_cvca1;
			// list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainStartingWith(mDv35_cvca4);

			QVERIFY(chain.isValid());
		}


		void onlyATandDV_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			// list << mCvca5_cvca5;
			// list << mCvca5_cvca4; do not include any CVCA certificates
			// list << mCvca4_cvca4;
			// list << mCvca4_cvca2;
			// list << mCvca2_cvca2;
			// list << mCvca2_cvca1;
			// list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca4_cvca2, mCvca4_cvca2));

			QVERIFY(chain.isValid());
		}


		void missingLinkCert_getChainStartingWith()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			// list << mCvca2_cvca1; do not include link certificate
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca1_cvca1);
			QVERIFY(!chain.isValid());

			chain = builder.getChainStartingWith(mCvca2_cvca2);
			QVERIFY(chain.isValid());

			chain = builder.getChainStartingWith(mCvca4_cvca2);
			QVERIFY(chain.isValid());
		}


		void missingLinkCert_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			// list << mCvca2_cvca1; do not include link certificate
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca1_cvca1, mCvca1_cvca1));
			QVERIFY(!chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca2_cvca2, mCvca2_cvca2));
			QVERIFY(chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca4_cvca2, mCvca4_cvca2));
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
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca1_cvca1);
			QVERIFY(chain.isValid());

			chain = builder.getChainStartingWith(mCvca2_cvca2);
			QVERIFY(chain.isValid());

			chain = builder.getChainStartingWith(mCvca4_cvca2);
			QVERIFY(chain.isValid());
		}


		void match_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca1_cvca1, mCvca1_cvca1));
			QVERIFY(chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca2_cvca2, mCvca2_cvca2));
			QVERIFY(chain.isValid());

			chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca4_cvca2, mCvca4_cvca2));
			QVERIFY(chain.isValid());
		}


		void noMatch_getChainStartingWith()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainStartingWith(mCvca5_cvca5);

			QVERIFY(!chain.isValid());
		}


		void noMatch_getChainForCertificationAuthority()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(createPaceOutput(mCvca5_cvca5, mCvca5_cvca5));

			QVERIFY(!chain.isValid());
		}


		void getChainForCertificationAuthority_forCarCurr()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);
			EstablishPaceChannelOutput output = createPaceOutput(mCvca4_cvca2, mCvca2_cvca2);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(output);

			QVERIFY(chain.isValid());
			QCOMPARE(chain.at(0)->getBody().getCertificationAuthorityReference(), output.getCarCurr());
			QCOMPARE(chain.size(), 2);
		}


		void getChainForCertificationAuthority_forCarPrev()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;
			CVCertificateChainBuilder builder(list);
			EstablishPaceChannelOutput output = createPaceOutput(mCvca5_cvca5, mCvca2_cvca2);

			CVCertificateChain chain = builder.getChainForCertificationAuthority(output);

			QVERIFY(chain.isValid());
			QCOMPARE(chain.at(0)->getBody().getCertificationAuthorityReference(), output.getCarPrev());
			QCOMPARE(chain.size(), 3);
		}


		void buildChains_removeDuplicates()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mDv35_cvca4;
			list << mAt38_dv35;

			CVCertificateChainBuilder builder(list);

			QCOMPARE(builder.mChains.size(), 2);
		}


		void buildChains_removeDuplicates_differentPointerWithSameCertificate()
		{
			const auto mCvca4_cvca2_duplicate = CVCTemplate::fromCvca("cvca4"_L1, "cvca2"_L1);
			const auto mCvca4_cvca4_duplicate = CVCTemplate::fromCvca("cvca4"_L1, "cvca4"_L1);

			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca4_cvca4_duplicate;
			list << mCvca4_cvca2_duplicate;
			list << mDv35_cvca4;
			list << mAt38_dv35;

			CVCertificateChainBuilder builder(list);

			QCOMPARE(builder.mChains.size(), 2);
		}


		void buildChains_differentInputOrder()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;

			QList<QSharedPointer<const CVCertificate>> list1;
			list1 << mAt38_dv35;
			list1 << mDv35_cvca4;
			list1 << mCvca1_cvca1;
			list1 << mCvca2_cvca1;
			list1 << mCvca4_cvca2;
			list1 << mCvca2_cvca2;
			list1 << mCvca4_cvca4;
			list1 << mCvca5_cvca4;
			list1 << mCvca5_cvca5;

			CVCertificateChainBuilder builder(list);
			CVCertificateChainBuilder builder1(list1);

			QCOMPARE(builder.mChains.size(), builder1.mChains.size());
		}


		void buildChains_maxChainDepthReached()
		{
			QTest::ignoreMessage(QtWarningMsg, "Maximum chain length exceeded, potential circular reference");

			auto cvca2_cvca4 = CVCTemplate::fromCvca("cvca2"_L1, "cvca4"_L1);
			QList<QSharedPointer<const CVCertificate>> list;
			list << cvca2_cvca4;
			list << mCvca4_cvca2;
			list << mDv35_cvca4;
			list << mAt38_dv35;

			CVCertificateChainBuilder builder(list);
		}


		void buildChains_moreThanOneLeaf()
		{
			QTest::ignoreMessage(QtWarningMsg, "More than one terminal certificate");

			QList<QSharedPointer<const CVCertificate>> list;
			list << mAt38_dv35;
			list << mAt20_dv5;

			CVCertificateChainBuilder builder(list);

			QCOMPARE(builder.mChains.size(), 0);
		}


		void buildChains_manyRoots()
		{
			// 1_1 -> 2_1 -> 4_2 -> 35_4 -> 38_35
			// 2_2 -> 4_2 -> 4_4 -> 35_4 -> 38_35
			// 4_4 -> 35_4 -> 38_35
			QList<QSharedPointer<const CVCertificate>> list;
			list << mCvca5_cvca5;
			list << mCvca5_cvca4;
			list << mCvca4_cvca4;
			list << mCvca4_cvca2;
			list << mCvca2_cvca2;
			list << mCvca2_cvca1;
			list << mCvca1_cvca1;
			list << mDv35_cvca4;
			list << mAt38_dv35;

			CVCertificateChainBuilder builder(list);

			auto checkRootCVC =
					[&](const QSharedPointer<const CVCertificate>& targetCVC)
					{
						return std::any_of(
								builder.mChains.begin(),
								builder.mChains.end(),
								[&targetCVC](const auto& chain)
								{
									return chain.first() == targetCVC;
								});
					};

			QCOMPARE(builder.mChains.size(), 3);
			QCOMPARE(checkRootCVC(mCvca1_cvca1), true);
			QCOMPARE(checkRootCVC(mCvca2_cvca2), true);
			QCOMPARE(checkRootCVC(mCvca4_cvca4), true);
		}


};


QTEST_GUILESS_MAIN(test_CVCertificateChainBuilder)
#include "test_CVCertificateChainBuilder.moc"
