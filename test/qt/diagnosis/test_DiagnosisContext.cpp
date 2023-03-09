/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "context/DiagnosisContext.h"

#include <QtTest>


using namespace governikus;


class test_DiagnosisContext
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_setTimestamp()
		{
			DiagnosisContext context;
			const QDateTime emptyTime = QDateTime();
			const QDateTime currentTime = QDateTime::currentDateTime();
			QSignalSpy spy(&context, &DiagnosisContext::timestampChanged);

			context.setTimestamp(emptyTime);
			QCOMPARE(context.getTimestamp(), emptyTime);
			QCOMPARE(spy.count(), 1);

			context.setTimestamp(currentTime);
			QCOMPARE(context.getTimestamp(), currentTime);
			QCOMPARE(spy.count(), 2);
		}


		void test_setReaderInfos()
		{
			DiagnosisContext context;
			const QString name1 = QStringLiteral("name");
			const QString name2 = QStringLiteral("name2");
			ReaderInfo info1(name1);
			ReaderInfo info2(name2);
			QVector<ReaderInfo> infoVector({info1, info2});
			QSignalSpy spy(&context, &DiagnosisContext::readerInfosChanged);

			context.setReaderInfos(infoVector);
			QCOMPARE(spy.count(), 1);
			context.setReaderInfos(infoVector);
			QCOMPARE(spy.count(), 2);
			QCOMPARE(context.getReaderInfos().size(), 2);
			QCOMPARE(context.getReaderInfos().value(0).getName(), name1);
			QCOMPARE(context.getReaderInfos().value(1).getName(), name2);
		}


		void test_setPscInfo()
		{
			DiagnosisContext context;
			const QString version = QStringLiteral("version");
			QVector<DiagnosisContext::ComponentInfo> components({DiagnosisContext::ComponentInfo()});
			QVector<DiagnosisContext::ComponentInfo> drivers({DiagnosisContext::ComponentInfo()});
			QSignalSpy spy(&context, &DiagnosisContext::pcscInfoChanged);

			context.setPcscInfo(version, components, drivers);
			QCOMPARE(spy.count(), 1);
			context.setPcscInfo(version, components, drivers);
			QCOMPARE(spy.count(), 2);

			QCOMPARE(context.getPcscComponents().size(), 1);
			QCOMPARE(context.getPcscDrivers().size(), 1);
			QCOMPARE(context.getPcscVersion(), version);
		}


		void test_ComponentInfo()
		{
			const QString path = QStringLiteral("path");
			const QString description = QStringLiteral("description");
			const QString version = QStringLiteral("version");
			const QString manufacturer = QStringLiteral("manufacturer");

			DiagnosisContext::ComponentInfo infoEmpty;
			DiagnosisContext::ComponentInfo info(path, description, version, manufacturer);

			QCOMPARE(infoEmpty.getPath(), QString());
			QCOMPARE(info.getPath(), path);

			QCOMPARE(infoEmpty.getDescription(), QString());
			QCOMPARE(info.getDescription(), description);

			QCOMPARE(infoEmpty.getVersion(), QString());
			QCOMPARE(info.getVersion(), version);
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisContext)
#include "test_DiagnosisContext.moc"
