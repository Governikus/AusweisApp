/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/DidAuthenticateMessage.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_DidAuthenticateMessage
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getter()
		{
			DidAuthenticateMessage msg(PaosType::UNKNOWN);

			msg.mDidName = QStringLiteral("dummy");

			auto handle = ConnectionHandle();
			handle.setIfdName(QStringLiteral("IFD dummy"));
			msg.mConnectionHandle = handle;

			QCOMPARE(msg.getDidName(), QStringLiteral("dummy"));
			QCOMPARE(msg.getConnectionHandle().getIfdName(), handle.getIfdName());
		}


		void setter()
		{
			DidAuthenticateMessage msg(PaosType::UNKNOWN);

			msg.setDidName(QStringLiteral("dummy"));

			auto handle = ConnectionHandle();
			handle.setIfdName(QStringLiteral("IFD dummy"));
			msg.setConnectionHandle(handle);

			QCOMPARE(msg.mDidName, QStringLiteral("dummy"));
			QCOMPARE(msg.mConnectionHandle.mIfdName, handle.getIfdName());
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateMessage)
#include "test_DidAuthenticateMessage.moc"
