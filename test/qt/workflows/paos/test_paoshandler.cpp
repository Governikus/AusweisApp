/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include <QByteArray>
#include <QFile>
#include <QtCore>
#include <QtTest>

#include "TestFileHelper.h"
#include "paos/PaosHandler.h"


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_paoshandler
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseDIDAuthenticateEAC1()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC1);
		}


		// test data of testbed (the attribute value of xsi:type contains a namespace identifier)
		void parseDIDAuthenticateEAC1_fromTestbed()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1_2.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC1);
		}


		void parseDIDAuthenticateEAC2()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC2);
		}


		void parseDIDAuthenticateEACAdditionalInputType()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEACAdditionalInput.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
		}


		void parseInitializeFramework()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/InitializeFramework.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::INITIALIZE_FRAMEWORK);
		}


		void parseStartPAOSResponse()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/StartPAOSResponse1.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::STARTPAOS_RESPONSE);
		}


		void parseTransmit()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/Transmit.xml"_L1);
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::TRANSMIT);
		}


};

QTEST_GUILESS_MAIN(test_paoshandler)
#include "test_paoshandler.moc"
