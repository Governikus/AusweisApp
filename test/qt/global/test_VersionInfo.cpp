/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "LogHandler.h"
#include "VersionInfo.h"


using namespace Qt::Literals::StringLiterals;
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
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void isNull()
		{
			QVERIFY(VersionInfo().isNull());
			QVERIFY(!VersionInfo::getInstance().isNull());
		}


		void toJson()
		{
			const auto data = VersionInfo::getInstance().toJson();
			QJsonParseError jsonError {};
			const auto json = QJsonDocument::fromJson(data, &jsonError);
			QCOMPARE(jsonError.error, QJsonParseError::NoError);

			const auto obj = json.object();
			QCOMPARE(obj["Name"_L1].toString(), QLatin1String("AusweisApp2"));
			QCOMPARE(obj["Specification-Title"_L1].toString(), QLatin1String("TR-03124-1"));
			QCOMPARE(obj["Specification-Version"_L1].toString(), QLatin1String("1.4"));
			QCOMPARE(obj["Specification-Vendor"_L1].toString(), QLatin1String("Federal Office for Information Security"));
			QCOMPARE(obj["Implementation-Title"_L1].toString(), QLatin1String("AusweisApp2"));
			QCOMPARE(obj["Implementation-Version"_L1].toString(), QLatin1String("x.y.z"));
			QCOMPARE(obj["Implementation-Vendor"_L1].toString(), QLatin1String("Governikus GmbH & Co. KG"));
		}


		void fromTextInvalidData()
		{
			auto versionInfo = VersionInfo::fromText(QLatin1StringView("skdkfsn sdnk fskdfn sdfk sflk nd\n"
																	   "\n"
																	   ":\n"
																	   "  :\r\t\n"
																	   ""));

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
			auto versionInfo = VersionInfo::fromText(QLatin1StringView("Name: MyName\n"
																	   "Specification-Title: MySpecTitle\n"
																	   "Specification-Version: MySpecVersion\n"
																	   "Specification-Vendor: MySpecVendor\n"
																	   "Implementation-Title: MyImplTitle\n"
																	   "Implementation-Version: MyImplVersion\n"
																	   "Implementation-Vendor: MyImplVendor"));

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

			QVERIFY(text.contains(QLatin1String("Name: AusweisApp2")));
			QVERIFY(text.contains(QLatin1String("Specification-Title: TR-03124-1")));
			QVERIFY(text.contains(QLatin1String("Specification-Version: 1.4")));
			QVERIFY(text.contains(QLatin1String("Specification-Vendor: Federal Office for Information Security")));
			QVERIFY(text.contains(QLatin1String("Implementation-Title: AusweisApp2")));
			QVERIFY(text.contains(QLatin1String("Implementation-Version: x.y.z")));
			QVERIFY(text.contains(QLatin1String("Implementation-Vendor: Governikus GmbH & Co. KG")));
		}


		void logging()
		{
			auto versionInfo = VersionInfo::getInstance();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			qDebug() << versionInfo;

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toByteArray().contains(versionInfo.toJson(QJsonDocument::JsonFormat::Compact)));
		}


};

QTEST_GUILESS_MAIN(test_VersionInfo)
#include "test_VersionInfo.moc"
