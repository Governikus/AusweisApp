/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ResourceLoader.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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

			for (const QString& file : std::as_const(mQmlFiles))
			{
				const QFileInfo info(file);
				QTest::newRow(info.fileName().toLatin1().data()) << info;
			}
		}


		void qmlFileStructureIsSane()
		{
			QFETCH(QFileInfo, qmlFile);

			QDir dir = qmlFile.dir();
			while (dir.dirName().startsWith('+'_L1))
			{
				dir.cdUp();
				const QString fileInParentFolder = dir.path() + QDir::separator() + qmlFile.fileName();
				QVERIFY(!mQmlFiles.contains(fileInParentFolder));
			}
		}


};

QTEST_GUILESS_MAIN(test_QmlFileStructure)
#include "test_QmlFileStructure.moc"
