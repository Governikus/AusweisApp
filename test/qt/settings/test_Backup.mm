/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
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

	private:
		enum class BackupState
		{
			Error,
			Excluded,
			Included
		};

		BackupState getBackupState(const QString& pFilename)
		{
			NSNumber* value = nil;
			NSError* error = nil;
			const bool success = [[NSURL fileURLWithPath: pFilename.toNSString()] getResourceValue: &value
					forKey: NSURLIsExcludedFromBackupKey
					error: &error];
			if (!success)
			{
				if (error == nil)
				{
					qCritical() << "getResourceValue failed without error description";
				}
				else
				{
					qCritical() << "getResourceValue failed with:" << error.localizedDescription;
				}
				return BackupState::Error;
			}
			return [value boolValue] ? BackupState::Excluded : BackupState::Included;
		}

	private Q_SLOTS:
		void disableBackup()
		{
			QTemporaryFile file;
			QVERIFY(file.open());
			const auto& fileName = file.fileName();
			QCOMPARE(getBackupState(fileName), BackupState::Included);

			auto settings = QSharedPointer<QSettings>::create(fileName, QSettings::NativeFormat);
			Backup::disable(settings);
			QCOMPARE(getBackupState(fileName), BackupState::Excluded);

			settings->setValue("test", 1);
			settings->sync();
			if (getBackupState(fileName) != BackupState::Included)
			{
				qCritical() << "File operation did NOT cause this property to reset to false.";
			}

			Backup::disable(settings);
			settings->sync();
			settings.reset();
			QCOMPARE(getBackupState(fileName), BackupState::Excluded);
		}


};

QTEST_GUILESS_MAIN(test_Backup)
#include "test_Backup.moc"
