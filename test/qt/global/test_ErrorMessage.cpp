/*!
 * \brief Unit tests for \ref ErrorMessage
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ErrorMessage.h"

#include <QtTest/QtTest>

using namespace governikus;

class test_ErrorMessage
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void toString()
		{
			QSKIP("Will be refactored in next release");

			auto str = ErrorMessage::toString(ErrorMessageId::PROXY_ERROR);
			QCOMPARE(str, QString("Establishing a connection via the proxy did not succeed.\nPlease contact Your administrator or our support at https://www.ausweisapp.bund.de/service/support/"));

			str = ErrorMessage::toString(ErrorMessageId::SSL_ERROR);
			QCOMPARE(str, QString("It wasn't possible to connect to the server: a secure connection could not be established.\nPlease contact Your service provider or our support at https://www.ausweisapp.bund.de/service/support/"));
		}


};

QTEST_GUILESS_MAIN(test_ErrorMessage)
#include "test_ErrorMessage.moc"
