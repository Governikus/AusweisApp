/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ResourceLoader.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QtTest>

using namespace governikus;

class test_QmlFileStructure
	: public QObject
{
	Q_OBJECT

	private:
		QStringList mQmlFiles;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();

			QDirIterator iterator(QStringLiteral(":"), {QStringLiteral("*.qml")}, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
			while (iterator.hasNext())
			{
				const auto& file = iterator.next();
				if (!file.endsWith(QStringLiteral("main.qml")))
				{
					mQmlFiles += file;
				}
			}
		}


		void qmlFileStructureIsSane_data()
		{
			QTest::addColumn<QFileInfo>("qmlFile");

			for (const QString& file : qAsConst(mQmlFiles))
			{
				const QFileInfo info(file);
				QTest::newRow(info.fileName().toLatin1().data()) << info;
			}
		}


		void qmlFileStructureIsSane()
		{
			QFETCH(QFileInfo, qmlFile);

			QDir dir = qmlFile.dir();
			while (dir.dirName().startsWith(QChar('+')))
			{
				dir.cdUp();
				const QString fileInParentFolder = dir.path() + QDir::separator() + qmlFile.fileName();
				QVERIFY(!mQmlFiles.contains(fileInParentFolder));
			}
		}


		void avoidMultipleDeviceFiles_data()
		{
			QTest::addColumn<QFileInfo>("qmlFile");

			for (const QString& file : qAsConst(mQmlFiles))
			{
				if (file.contains(QStringLiteral("+phone")) || file.contains(QStringLiteral("+tablet")))
				{
					const QFileInfo info(file);
					QTest::newRow(info.fileName().toLatin1().data()) << info;
				}
			}
		}


		void avoidMultipleDeviceFiles()
		{
			QFETCH(QFileInfo, qmlFile);

			QDir dir = qmlFile.dir();
			const auto& device = dir.dirName();
			dir.cdUp();

			const auto& parentFolder = dir.dirName();
			if (parentFolder == QStringLiteral("+android") || parentFolder == QStringLiteral("+ios"))
			{
				dir.cdUp();
				dir.cd(device);
				const QString fileInParentFolder = dir.path() + QDir::separator() + qmlFile.fileName();
				QVERIFY(!mQmlFiles.contains(fileInParentFolder));
				return;
			}

			const QString file = QDir::separator() + device + QDir::separator() + qmlFile.fileName();
			QVERIFY(!mQmlFiles.contains(dir.path() + QDir::separator() + QStringLiteral("+android") + file));
			QVERIFY(!mQmlFiles.contains(dir.path() + QDir::separator() + QStringLiteral("+ios") + file));
		}


};

QTEST_GUILESS_MAIN(test_QmlFileStructure)
#include "test_QmlFileStructure.moc"
