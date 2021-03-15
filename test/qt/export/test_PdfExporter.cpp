/*!
 * \brief Unit tests for \ref PdfExporter
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "PdfExporter.h"

#include "AppSettings.h"
#include "ResourceLoader.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(QVector<HistoryInfo>)
using Pair = QPair<QString, QString>;
Q_DECLARE_METATYPE(QVector<Pair>)

class test_PdfExporter
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void history_data()
		{
			QTest::addColumn<int>("min");
			QTest::addColumn<int>("max");
			QTest::addColumn<QVector<HistoryInfo> >("infos");

			QVector<HistoryInfo> entries;

			QTest::newRow("empty") << 10000 << 100000 << entries;

			for (int i = 0; i < 100; ++i)
			{
				entries << HistoryInfo("SubjectName", "SubjectUrl", "Usage", QDateTime::currentDateTime(), "TermOfUsage", {"RequestedData"});
			}
			QTest::newRow("multiple") << 50000 << 350000 << entries;
		}


		void history()
		{
			QFETCH(int, min);
			QFETCH(int, max);
			QFETCH(QVector<HistoryInfo>, infos);

			Env::getSingleton<AppSettings>()->getHistorySettings().setHistoryInfos(infos);
			Env::getSingleton<AppSettings>()->getHistorySettings().save();

			QTemporaryFile file;
			QVERIFY(file.open());

			PdfExporter exporter(file.fileName(), false, false);
			QVERIFY(exporter.exportHistory());
			const QFileInfo info(file.fileName());
			qDebug() << info.size();
			QVERIFY(info.size() > min);
			QVERIFY(info.size() < max);
		}


		void selfInfo_data()
		{
			QTest::addColumn<int>("min");
			QTest::addColumn<int>("max");
			QTest::addColumn<QVector<Pair> >("data");

			QVector<QPair<QString, QString> > data;

			QTest::newRow("empty") << 10000 << 100000 << data;

			for (int i = 0; i < 100; ++i)
			{
				data << QPair<QString, QString>(QString("key"), QString("value"));
			}
			QTest::newRow("filled") << 25000 << 150000 << data;
		}


		void selfInfo()
		{
			QFETCH(int, min);
			QFETCH(int, max);
			QFETCH(QVector<Pair>, data);

			const auto& now = QDateTime::currentDateTime();

			QTemporaryFile file;
			QVERIFY(file.open());

			PdfExporter exporter(file.fileName(), false, false);
			QVERIFY(exporter.exportSelfInfo(now, data));
			const QFileInfo info(file.fileName());
			qDebug() << info.size();
			QVERIFY(info.size() > min);
			QVERIFY(info.size() < max);
		}


};

QTEST_MAIN(test_PdfExporter)
#include "test_PdfExporter.moc"
