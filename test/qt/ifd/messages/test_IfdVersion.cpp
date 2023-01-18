/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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
			QCOMPARE(IfdVersion("IFDInterface_WebSocket_Unknown"), IfdVersion::Version::Unknown);
			QCOMPARE(IfdVersion("IFDInterface_WebSocket_v0"), IfdVersion::Version::v0);
			QCOMPARE(IfdVersion("IFDInterface_WebSocket_v2"), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion("IFDInterface_WebSocket_v9001"), IfdVersion::Version::Unknown);
		}


		void isValid()
		{
			QCOMPARE(IfdVersion(IfdVersion::Version::Unknown).isValid(), false);
			QCOMPARE(IfdVersion(IfdVersion::Version::v0).isValid(), true);
			QCOMPARE(IfdVersion(IfdVersion::Version::v2).isValid(), true);
		}


		void isSupported()
		{
			QCOMPARE(IfdVersion(IfdVersion::Version::Unknown).isSupported(), false);
			QCOMPARE(IfdVersion(IfdVersion::Version::v0).isSupported(), false);
			QCOMPARE(IfdVersion(IfdVersion::Version::v2).isSupported(), true);
		}


		void supportedVersions()
		{
			QVector<IfdVersion::Version> versions({IfdVersion::Version::v2});
			if (IfdVersion(IfdVersion::Version::v0).isSupported())
			{
				versions.prepend(IfdVersion::Version::v0);
			}
			QCOMPARE(IfdVersion::supported(), versions);
		}


		void selectSupportedVersions()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			const IfdVersion::Version expected = v0Supported ? IfdVersion::Version::v0 : IfdVersion::Version::Unknown;

			QCOMPARE(IfdVersion::selectLatestSupported({}), IfdVersion::Version::Unknown);

			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::Unknown}), IfdVersion::Version::Unknown);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0}), expected);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v2}), IfdVersion::Version::v2);

			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::Unknown, IfdVersion::Version::v0}), expected);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0, IfdVersion::Version::Unknown}), expected);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::Unknown, IfdVersion::Version::v2}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v2, IfdVersion::Version::Unknown}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0, IfdVersion::Version::v2}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v2, IfdVersion::Version::v0}), IfdVersion::Version::v2);

			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::Unknown, IfdVersion::Version::v0, IfdVersion::Version::v2}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::Unknown, IfdVersion::Version::v2, IfdVersion::Version::v0}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0, IfdVersion::Version::Unknown, IfdVersion::Version::v2}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v0, IfdVersion::Version::v2, IfdVersion::Version::Unknown}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v2, IfdVersion::Version::Unknown, IfdVersion::Version::v0}), IfdVersion::Version::v2);
			QCOMPARE(IfdVersion::selectLatestSupported({IfdVersion::Version::v2, IfdVersion::Version::v0, IfdVersion::Version::Unknown}), IfdVersion::Version::v2);
		}


};

QTEST_GUILESS_MAIN(test_IfdVersion)
#include "test_IfdVersion.moc"
