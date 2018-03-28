/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include <QByteArray>
#include <QFile>
#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "paos/PaosHandler.h"
#include "TestFileHelper.h"


using namespace governikus;


class test_paoshandler
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseDIDAuthenticateEAC1()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC1);
		}


		// test data of testbed (the attribute value of xsi:type contains a namespace identifier)
		void parseDIDAuthenticateEAC1_fromTestbed()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1_2.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC1);
		}


		void parseDIDAuthenticateEAC2()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC2);
		}


		void parseDIDAuthenticateEACAdditionalInputType()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDAuthenticateEACAdditionalInput.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE);
		}


		void parseDIDList()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/DIDList.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DID_LIST);
		}


		void parseDisconnect()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/Disconnect.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::DISCONNECT);
		}


		void parseInitializeFramework()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/InitializeFramework.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::INITIALIZE_FRAMEWORK);
		}


		void parseStartPAOSResponse()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/StartPAOSResponse1.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::STARTPAOS_RESPONSE);
		}


		void parseTransmit()
		{
			QByteArray initFW = TestFileHelper::readFile(":/paos/Transmit.xml");
			PaosHandler handler(initFW);
			QVERIFY(handler.getDetectedPaosType() == PaosType::TRANSMIT);
		}


};

QTEST_GUILESS_MAIN(test_paoshandler)
#include "test_paoshandler.moc"
