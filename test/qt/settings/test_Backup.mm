/*!
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "Backup.h"

#include <QTemporaryFile>
#include <QTest>

#if defined(Q_OS_IOS)
	#import <UIKit/UIKit.h>
#else
	#import <AppKit/AppKit.h>
#endif


using namespace governikus;


class test_Backup
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void disableBackup()
		{
			QTemporaryFile file;
			QVERIFY(file.open());
			const auto& fileName = file.fileName();

			bool success = false;
			NSNumber* value = nil;
			NSError* error = nil;

			success = [[NSURL fileURLWithPath: fileName.toNSString()] getResourceValue: &value
					forKey: NSURLIsExcludedFromBackupKey
					error: &error];
			QVERIFY(success);
			QCOMPARE(error, nil);
			QCOMPARE([value boolValue], NO);

			auto settings = QSharedPointer<QSettings>::create(fileName, QSettings::NativeFormat);
			Backup::disable(settings);

			success = [[NSURL fileURLWithPath: fileName.toNSString()] getResourceValue: &value
					forKey: NSURLIsExcludedFromBackupKey
					error: &error];
			QVERIFY(success);
			QCOMPARE(error, nil);
			QCOMPARE([value boolValue], YES);

			settings->setValue("test", 1);
			settings->sync();

			success = [[NSURL fileURLWithPath: fileName.toNSString()] getResourceValue: &value
					forKey: NSURLIsExcludedFromBackupKey
					error: &error];
			QVERIFY(success);
			QCOMPARE(error, nil);
			QCOMPARE([value boolValue], NO);

			Backup::disable(settings);
			settings->sync();
			settings.reset();

			success = [[NSURL fileURLWithPath: fileName.toNSString()] getResourceValue: &value
					forKey: NSURLIsExcludedFromBackupKey
					error: &error];
			QVERIFY(success);
			QCOMPARE(error, nil);
			QCOMPARE([value boolValue], YES);
		}


};

QTEST_GUILESS_MAIN(test_Backup)
#include "test_Backup.moc"
