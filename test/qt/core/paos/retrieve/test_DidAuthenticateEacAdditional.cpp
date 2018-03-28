/*!
 * \brief Unit tests for \DidAuthenticateEACAdditionalInputType
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "TestFileHelper.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "paos/retrieve/DidAuthenticateEacAdditionalParser.h"


using namespace governikus;

class test_DidAuthenticateEacAdditional
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void parse()
		{
			QByteArray content = TestFileHelper::readFile(":paos/DIDAuthenticateEACAdditionalInput.xml");
			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);

			QScopedPointer<DIDAuthenticateEACAdditional> eacAdd(static_cast<DIDAuthenticateEACAdditional*>(paosMessage));

			QVERIFY(eacAdd);
			QCOMPARE(eacAdd->getSignature(), QString("86E24ACCE25E2B4623A3558EED7AA65658FEE70CC7B46CB9D34BD50B2F27E1E2221473D5B871497563693E0F28BE9C8896062A9C02CE3959AD4BD58DEA0E984B"));
		}


		void noSignature()
		{
			QByteArray content = TestFileHelper::readFile(":paos/DIDAuthenticateEACAdditionalInput_noSignature.xml");
			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);

			QVERIFY(paosMessage == nullptr);
		}


		void manySignature()
		{
			QByteArray content = TestFileHelper::readFile(":paos/DIDAuthenticateEACAdditionalInput_manySignature.xml");
			PaosMessage* paosMessage = DidAuthenticateEacAdditionalParser().parse(content);

			QVERIFY(paosMessage == nullptr);
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEacAdditional)
#include "test_DidAuthenticateEacAdditional.moc"
