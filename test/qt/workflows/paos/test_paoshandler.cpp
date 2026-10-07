/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/PaosHandler.h"

#include "TestFileHelper.h"

#include <QByteArray>
#include <QFile>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_paoshandler
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseDIDAuthenticateEAC1()
		{
			const auto file = TestFileHelper::getFile(":/paos/DIDAuthenticateEAC1.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC1);
		}


		// test data of testbed (the attribute value of xsi:type contains a namespace identifier)
		void parseDIDAuthenticateEAC1_fromTestbed()
		{
			const auto file = TestFileHelper::getFile(":/paos/DIDAuthenticateEAC1_2.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC1);
		}


		void parseDIDAuthenticateEAC2()
		{
			const auto file = TestFileHelper::getFile(":/paos/DIDAuthenticateEAC2.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC2);
		}


		void parseDIDAuthenticateEACAdditionalInputType()
		{
			const auto file = TestFileHelper::getFile(":/paos/DIDAuthenticateEACAdditionalInput.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
		}


		void parseInitializeFramework()
		{
			const auto file = TestFileHelper::getFile(":/paos/InitializeFramework.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::INITIALIZE_FRAMEWORK);
		}


		void parseStartPAOSResponse()
		{
			const auto file = TestFileHelper::getFile(":/paos/StartPAOSResponse1.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::STARTPAOS_RESPONSE);
		}


		void parseTransmit()
		{
			const auto file = TestFileHelper::getFile(":/paos/Transmit.xml"_L1);
			PaosHandler handler(file.data(), true);
			QVERIFY(handler.getDetectedPaosType() == PaosType::TRANSMIT);
		}


};

QTEST_GUILESS_MAIN(test_paoshandler)
#include "test_paoshandler.moc"
