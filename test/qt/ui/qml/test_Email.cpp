/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "Email.h"

#include <QCoreApplication>
#include <QRegularExpression>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_Email
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			QCoreApplication::setOrganizationName("orgaName"_L1);
			QCoreApplication::setOrganizationDomain("orgaDomain"_L1);
			QCoreApplication::setApplicationVersion("appVersion"_L1);
		}


		void test_generateMailBody_data()
		{
			QTest::addColumn<QString>("serviceUrl");
			QTest::addColumn<bool>("percentEncoding");
			QTest::addColumn<QString>("result");

			const auto& resultWithService = uR"(Please describe the error that occurred.


Application: Test_ui_qml_Email
Application%20Version: appVersion
Organization: orgaName
Organization%20Domain: orgaDomain
System: .+
Kernel: .+
Architecture: .+
Device: .+
Qt%20Version: .+
OpenSSL%20Version: .+

Error code:
Card_Communication_Error

Service URL:
https://www.foo.bar

Parameter of occurred error:
https://www.test.de)"_s;
			QTest::newRow("service - percent") << QStringLiteral("https://www.foo.bar") << true << resultWithService;
			QTest::newRow("service - no percent") << QStringLiteral("https://www.foo.bar") << false << QString(resultWithService).replace("%20"_L1, " "_L1);

			const auto resultWithoutService = QString(resultWithService).remove(292, 34);
			QTest::newRow("no service - percent") << QString() << true << resultWithoutService;
			QTest::newRow("no service - no percent") << QString() << false << QString(resultWithoutService).replace("%20"_L1, " "_L1);
		}


		void test_generateMailBody()
		{
			QFETCH(QString, serviceUrl);
			QFETCH(bool, percentEncoding);
			QFETCH(QString, result);

			const GlobalStatus::ExternalInfoMap infoMap {
				{GlobalStatus::ExternalInformation::LAST_URL, "https://www.test.de"_L1}
			};
			const GlobalStatus globalStatus(GlobalStatus::Code::Card_Communication_Error, infoMap);
			const auto& mailBody = generateMailBody(globalStatus, serviceUrl, percentEncoding);

			const QRegularExpression re(result);
			QVERIFY(re.match(mailBody).hasMatch());
		}


};

QTEST_GUILESS_MAIN(test_Email)
#include "test_Email.moc"
