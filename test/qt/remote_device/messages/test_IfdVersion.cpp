/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdVersion.h"

#include <QtTest>

using namespace governikus;


class test_IfdVersion
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void stringParsing()
		{
			QCOMPARE(IfdVersion::fromString("IFDInterface_WebSocket_v0"), IfdVersion::Version::v0);

			QCOMPARE(IfdVersion::fromString("IFDInterface_WebSocket_v9001"), IfdVersion::Version::Unknown);
		}


		void isValid()
		{
			QCOMPARE(IfdVersion(IfdVersion::Version::Unknown).isValid(), false);
			QCOMPARE(IfdVersion(IfdVersion::Version::v0).isValid(), true);
		}


		void isSupported()
		{
			QCOMPARE(IfdVersion(IfdVersion::Version::Unknown).isSupported(), false);
			QCOMPARE(IfdVersion(IfdVersion::Version::v0).isSupported(), true);
		}


		void supportedVersions()
		{
			QCOMPARE(IfdVersion::supported(), {IfdVersion::Version::v0});
		}


		void selectSupportedVersions()
		{
			QCOMPARE(IfdVersion::selectLatestSupported({}), IfdVersion::Version::Unknown);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v_test}), IfdVersion::Version::Unknown);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0}), IfdVersion::Version::v0);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0, IfdVersion::Version::v_test}), IfdVersion::Version::v0);
		}


};

QTEST_GUILESS_MAIN(test_IfdVersion)
#include "test_IfdVersion.moc"
