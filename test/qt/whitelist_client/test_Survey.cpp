/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Survey.h"

#include <QtTest/QtTest>

using namespace governikus;


class test_Survey
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testValidInvalid()
		{
			const Survey nonNullSurvey(
				QStringLiteral("37.0.A.2.248"),
				QStringLiteral("6.0"),
				QStringLiteral("3.18.19+"),
				128,
				QStringLiteral("Sony"),
				QStringLiteral("F3311"),
				QStringLiteral("XPERIA E5"),
				QStringLiteral("1.15.1"));
			QVERIFY(!nonNullSurvey.isNull());

			const Survey nullSurvey;
			QVERIFY(nullSurvey.isNull());
		}


		void testToJSON()
		{
			const Survey survey(
				QStringLiteral("37.0.A.2.248"),
				QStringLiteral("6.0"),
				QStringLiteral("3.18.19+"),
				128,
				QStringLiteral("Sony"),
				QStringLiteral("F3311"),
				QStringLiteral("XPERIA E5"),
				QStringLiteral("1.15.1"));

			const QByteArray& byteArray = survey.toJsonByteArray();
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"AusweisAppVersionNumber\": \"1.15.1\",\n"
							   "    \"ModelName\": \"XPERIA E5\",\n"
							   "    \"ModelNumber\": \"F3311\",\n"
							   "    \"Rom\": {\n"
							   "        \"AndroidVersion\": \"6.0\",\n"
							   "        \"BuildNumber\": \"37.0.A.2.248\",\n"
							   "        \"KernelVersion\": \"3.18.19+\",\n"
							   "        \"MaximumNfcPacketLength\": 128\n"
							   "    },\n"
							   "    \"Vendor\": \"Sony\"\n"
							   "}\n"));
		}


};

QTEST_GUILESS_MAIN(test_Survey)
#include "test_Survey.moc"
