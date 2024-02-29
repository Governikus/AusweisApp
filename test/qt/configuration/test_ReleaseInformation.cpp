/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ReleaseInformation
 */

#include "ReleaseInformation.h"

#include <QTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_ReleaseInformation
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			QCoreApplication::setApplicationVersion("1.2.3"_L1);
		}


		void test_requiresInitialUpdate()
		{
			ReleaseInformation information;
			QCOMPARE(information.requiresInitialUpdate(), true);
		}


		void test_versionNumber()
		{
			ReleaseInformation information;
			QCOMPARE(information.versionNumber(), VersionNumber::getApplicationVersion());
		}


		void test_explicitVersionNumber()
		{
			ReleaseInformation information(VersionNumber("1.2.4"_L1));
			QCOMPARE(information.versionNumber(), VersionNumber("1.2.4"_L1));
		}


		void test_pathReleaseNotes()
		{
			ReleaseInformation information;
			QCOMPARE(information.pathReleaseNotes().size(), 4);
		}


		void test_pathReleaseNotesForExplicitVersion()
		{
			ReleaseInformation information(VersionNumber("1.2.4"_L1));
			QCOMPARE(information.pathReleaseNotes().size(), 5);
		}


		void test_pathReleaseNotesForExplicitVersionOnly()
		{
			ReleaseInformation information(VersionNumber("1.2.4"_L1), true);
			QCOMPARE(information.pathReleaseNotes().size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_ReleaseInformation)
#include "test_ReleaseInformation.moc"
