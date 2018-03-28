/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "LogHandler.h"
#include "VersionInfo.h"


using namespace governikus;


class test_VersionInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			QCoreApplication::setOrganizationName(QStringLiteral("Governikus GmbH & Co. KG"));
			QCoreApplication::setApplicationVersion(QStringLiteral("x.y.z"));
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void isNull()
		{
			QVERIFY(VersionInfo().isNull());
			QVERIFY(!VersionInfo::getInstance().isNull());
		}


		void toJson()
		{
			auto json = VersionInfo::getInstance().toJson();
			QSharedPointer<QJsonParseError> parseError(new QJsonParseError);
			auto obj = QJsonDocument::fromJson(json, parseError.data()).object();

			QCOMPARE(parseError->error, QJsonParseError::ParseError::NoError);
			QCOMPARE(obj["Name"].toString(), QLatin1String("Test_global_VersionInfo"));
			QCOMPARE(obj["Specification-Title"].toString(), QLatin1String("TR-03124"));
			QCOMPARE(obj["Specification-Version"].toString(), QLatin1String("1.3"));
			QCOMPARE(obj["Specification-Vendor"].toString(), QLatin1String("Federal Office for Information Security"));
			QCOMPARE(obj["Implementation-Title"].toString(), QLatin1String("Test_global_VersionInfo"));
			QCOMPARE(obj["Implementation-Version"].toString(), QLatin1String("x.y.z"));
			QCOMPARE(obj["Implementation-Vendor"].toString(), QLatin1String("Governikus GmbH & Co. KG"));
		}


		void fromTextInvalidData()
		{
			auto versionInfo = VersionInfo::fromText("skdkfsn sdnk fskdfn sdfk sflk nd\n"
													 "\n"
													 ":\n"
													 "  :\r\t\n"
													 "");

			QVERIFY(versionInfo.getName().isNull());
			QVERIFY(versionInfo.getSpecificationTitle().isNull());
			QVERIFY(versionInfo.getSpecificationVersion().isNull());
			QVERIFY(versionInfo.getSpecificationVendor().isNull());
			QVERIFY(versionInfo.getImplementationTitle().isNull());
			QVERIFY(versionInfo.getImplementationVersion().isNull());
			QVERIFY(versionInfo.getImplementationVendor().isNull());
		}


		void fromText()
		{
			auto versionInfo = VersionInfo::fromText("Name: MyName\n"
													 "Specification-Title: MySpecTitle\n"
													 "Specification-Version: MySpecVersion\n"
													 "Specification-Vendor: MySpecVendor\n"
													 "Implementation-Title: MyImplTitle\n"
													 "Implementation-Version: MyImplVersion\n"
													 "Implementation-Vendor: MyImplVendor");

			QCOMPARE(versionInfo.getName(), QLatin1String("MyName"));
			QCOMPARE(versionInfo.getSpecificationTitle(), QLatin1String("MySpecTitle"));
			QCOMPARE(versionInfo.getSpecificationVersion(), QLatin1String("MySpecVersion"));
			QCOMPARE(versionInfo.getSpecificationVendor(), QLatin1String("MySpecVendor"));
			QCOMPARE(versionInfo.getImplementationTitle(), QLatin1String("MyImplTitle"));
			QCOMPARE(versionInfo.getImplementationVersion(), QLatin1String("MyImplVersion"));
			QCOMPARE(versionInfo.getImplementationVendor(), QLatin1String("MyImplVendor"));
		}


		void toText()
		{
			auto text = VersionInfo::getInstance().toText();

			QVERIFY(text.contains(QLatin1String("Name: Test_global_VersionInfo")));
			QVERIFY(text.contains(QLatin1String("Specification-Title: TR-03124")));
			QVERIFY(text.contains(QLatin1String("Specification-Version: 1.3")));
			QVERIFY(text.contains(QLatin1String("Specification-Vendor: Federal Office for Information Security")));
			QVERIFY(text.contains(QLatin1String("Implementation-Title: Test_global_VersionInfo")));
			QVERIFY(text.contains(QLatin1String("Implementation-Version: x.y.z")));
			QVERIFY(text.contains(QLatin1String("Implementation-Vendor: Governikus GmbH & Co. KG")));
		}


		void logging()
		{
			auto versionInfo = VersionInfo::getInstance();
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);

			qDebug() << versionInfo;

			QCOMPARE(spy.count(), 1);
			QVERIFY(spy.takeFirst().at(0).toByteArray().contains(versionInfo.toJson(QJsonDocument::JsonFormat::Compact)));
		}


};

QTEST_GUILESS_MAIN(test_VersionInfo)
#include "test_VersionInfo.moc"
