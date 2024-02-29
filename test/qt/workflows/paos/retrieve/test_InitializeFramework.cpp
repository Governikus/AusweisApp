/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \InitializeFramework
 */

#include "TestFileHelper.h"
#include "paos/retrieve/InitializeFramework.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_InitializeFramework
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void parse()
		{
			QByteArray content = TestFileHelper::readFile(":paos/InitializeFramework_withMessageID.xml"_L1);

			InitializeFramework message(content);

			QCOMPARE(message.getMessageId(), "urn:uuid:c0f05ac0-1a67-4a0b-acbd-78309fcdb002"_L1);
			QCOMPARE(message.getRelatesTo(), QString());
		}


};

QTEST_GUILESS_MAIN(test_InitializeFramework)
#include "test_InitializeFramework.moc"
