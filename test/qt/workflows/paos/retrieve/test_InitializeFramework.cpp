/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/InitializeFramework.h"

#include "paos/retrieve/PaosParser.h"

#include "TestParserHelper.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_InitializeFramework
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parse()
		{
			auto parser = TestParserHelper::create(":paos/InitializeFramework_withMessageID.xml"_L1);
			auto* pm = PaosParser().parse(parser).release();
			const std::unique_ptr<InitializeFramework> frameworkMessage(static_cast<InitializeFramework*>(pm));
			QCOMPARE(frameworkMessage->mType, PaosType::INITIALIZE_FRAMEWORK);
			QCOMPARE(frameworkMessage->getMessageId(), "urn:uuid:c0f05ac0-1a67-4a0b-acbd-78309fcdb002"_L1);
			QCOMPARE(frameworkMessage->getRelatesTo(), QString());
		}


};

QTEST_GUILESS_MAIN(test_InitializeFramework)
#include "test_InitializeFramework.moc"
