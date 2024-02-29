/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref PcscReaderManagerPlugIn
 */

#include "PcscReaderManagerPlugIn.h"

#include <QtTest>

using namespace governikus;


class test_PcscReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void addReaders()
		{
#if !(defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)) && !defined(Q_OS_FREEBSD)
			QSKIP("Using LD_PRELOAD is not supported");
#endif

			const QString readerName = QStringLiteral("PCSC");

			PcscReaderManagerPlugIn plugin;
			QCOMPARE(plugin.objectName(), QStringLiteral("PcscReaderManager"));
			QVERIFY(plugin.getReaders().isEmpty());

			QSignalSpy spyAdded(&plugin, &PcscReaderManagerPlugIn::fireReaderAdded);
			QTest::ignoreMessage(QtDebugMsg, QStringLiteral("fireReaderAdded: \"%1\" ( 1 reader in total )").arg(readerName).toUtf8().data());
			plugin.addReaders({readerName});
			QCOMPARE(spyAdded.size(), 1);
			QCOMPARE(plugin.getReaders().size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_PcscReaderManagerPlugIn)
#include "test_PcscReaderManagerPlugIn.moc"
