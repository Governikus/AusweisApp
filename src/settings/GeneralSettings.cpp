/*!
 * GeneralSettings.cpp
 *
 * \brief Contains the method definitions of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "GeneralSettings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>
#include <QtGlobal>

#if defined(Q_OS_OSX)
#import <Cocoa/Cocoa.h>
#include <QRegularExpression>
#endif

#ifndef QT_NO_DEBUG
static bool Test_AutoStart = GENERAL_SETTINGS_DEFAULT_AUTOSTART;
#endif

using namespace governikus;

const QLatin1String SETTINGS_NAME_AUTO_CLOSE_WINDOW("autoCloseWindow");
const QLatin1String SETTINGS_NAME_SHOW_SETUP_ASSISTANT("showSetupAssistant");
const QLatin1String SETTINGS_NAME_REMIND_USER_TO_CLOSE("remindToClose");
const QLatin1String SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION("persistentSettingsVersion");
const QLatin1String SETTINGS_NAME_TRANSPORT_PIN_REMINDER("transportPinReminder");
const QLatin1String SETTINGS_NAME_DEVELOPER_MODE("developerMode");
const QLatin1String SETTINGS_NAME_USE_SELFAUTH_TEST_URI("selfauthTestUri");

const QLatin1String SETTINGS_GROUP_NAME_GENERAL("common");
const QLatin1String SETTINGS_NAME_AUTO("autoUpdateCheck");
const QLatin1String SETTINGS_NAME_KEYLESS_PASSWORD("keylessPassword");


GeneralSettings::GeneralSettings()
	: AbstractSettings()
	, mAutoCloseWindowAfterAuthentication(true)
	, mAutoUpdateCheck(true)
	, mUseScreenKeyboard(false)
	, mShowSetupAssistant(true)
	, mRemindUserToClose(true)
	, mPersistentSettingsVersion()
	, mTransportPinReminder(true)
	, mDeveloperMode(false)
	, mSelfauthenticationTestUri(false)
{
	// QFuture.result() crashes under linux and win if uninitalized
	mAutoStart = QtConcurrent::run([] {
		return GENERAL_SETTINGS_DEFAULT_AUTOSTART;
	});
}


GeneralSettings::~GeneralSettings()
{
	mAutoStart.waitForFinished();
}


void GeneralSettings::load()
{
	auto settings = getStore();
	// Check if the key "autoCloseWindow" (introduced in changeset 199210b0b20c)
	// does not yet exist to detect a new installation. This key was the first one
	// in the settings general group.
	const bool isNewInstallation = !settings->allKeys().contains(SETTINGS_NAME_AUTO_CLOSE_WINDOW);

	// for historical reasons this is not loaded/saved in the general settings group
	mAutoCloseWindowAfterAuthentication = settings->value(SETTINGS_NAME_AUTO_CLOSE_WINDOW, mAutoCloseWindowAfterAuthentication).toBool();
	mShowSetupAssistant = settings->value(SETTINGS_NAME_SHOW_SETUP_ASSISTANT, mShowSetupAssistant).toBool();
	mRemindUserToClose = settings->value(SETTINGS_NAME_REMIND_USER_TO_CLOSE, mRemindUserToClose).toBool();
	mPersistentSettingsVersion = settings->value(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION, mPersistentSettingsVersion).toString();
	mTransportPinReminder = settings->value(SETTINGS_NAME_TRANSPORT_PIN_REMINDER, isNewInstallation).toBool();
	mDeveloperMode = settings->value(SETTINGS_NAME_DEVELOPER_MODE, mDeveloperMode).toBool();
	mSelfauthenticationTestUri = settings->value(SETTINGS_NAME_USE_SELFAUTH_TEST_URI, mSelfauthenticationTestUri).toBool();

	settings->beginGroup(SETTINGS_GROUP_NAME_GENERAL);

	mAutoUpdateCheck = settings->value(SETTINGS_NAME_AUTO, mAutoUpdateCheck).toBool();
	mUseScreenKeyboard = settings->value(SETTINGS_NAME_KEYLESS_PASSWORD, mUseScreenKeyboard).toBool();

	settings->endGroup();

#if defined(Q_OS_WIN32) || defined(Q_OS_OSX)
	if (isNewInstallation)
	{
		setAutoStart(GENERAL_SETTINGS_DEFAULT_AUTOSTART);
	}
	else
#endif
	{
		// Iterate autostart entries in order to remove broken login items on macos.
		// This process might take up to 15s per entry.
		mAutoStart.waitForFinished();
		mAutoStart = QtConcurrent::run(readSettingsAutoStart);
	}
}


bool GeneralSettings::isUnsaved() const
{
	GeneralSettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void GeneralSettings::save()
{
	auto settings = getStore();

	// for historical reasons this is not loaded/saved in the general settings group
	settings->setValue(SETTINGS_NAME_AUTO_CLOSE_WINDOW, mAutoCloseWindowAfterAuthentication);
	settings->setValue(SETTINGS_NAME_SHOW_SETUP_ASSISTANT, mShowSetupAssistant);
	settings->setValue(SETTINGS_NAME_REMIND_USER_TO_CLOSE, mRemindUserToClose);

	mPersistentSettingsVersion = QCoreApplication::applicationVersion();
	settings->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION, mPersistentSettingsVersion);
	settings->setValue(SETTINGS_NAME_TRANSPORT_PIN_REMINDER, mTransportPinReminder);
	settings->setValue(SETTINGS_NAME_DEVELOPER_MODE, mDeveloperMode);
	settings->setValue(SETTINGS_NAME_USE_SELFAUTH_TEST_URI, mSelfauthenticationTestUri);

	settings->beginGroup(SETTINGS_GROUP_NAME_GENERAL);

	settings->setValue(SETTINGS_NAME_AUTO, mAutoUpdateCheck);
	settings->setValue(SETTINGS_NAME_KEYLESS_PASSWORD, mUseScreenKeyboard);
	writeSettingsAutoStart(mAutoStart);

	settings->endGroup();
	settings->sync();
}


#if defined(Q_OS_OSX)

bool addAutoStartMacOS()
{
	QRegularExpression regex("/Contents/Resources$");
	NSString* path = QCoreApplication::applicationDirPath().remove(regex).toNSString();
	CFURLRef url = (CFURLRef)[NSURL fileURLWithPath: path];

	LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);
	if (loginItems)
	{
		LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(loginItems, kLSSharedFileListItemLast, NULL, NULL, url, NULL, NULL);
		if (item)
		{
			CFRelease(item);
			return true;
		}
	}
	return false;
}


bool checkAndRemoveAutoStartMacOS(bool pRemove = true)
{
	qDebug() << "Loading OSX login items";

	QRegularExpression regex("/Contents/Resources$");
	NSString* appPath = QCoreApplication::applicationDirPath().remove(regex).toNSString();
	CFURLRef url = (CFURLRef)[NSURL fileURLWithPath: appPath];

	// Create a reference to the shared file list.
	LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
			kLSSharedFileListSessionLoginItems, NULL);

	if (loginItems)
	{
		//Retrieve the list of Login Items and cast them to
		// a NSArray so that it will be easier to iterate.
		NSArray* loginItemsArray = (NSArray*) LSSharedFileListCopySnapshot(loginItems, NULL);
		for (NSUInteger i = 0; i < [loginItemsArray count]; ++i)
		{
			LSSharedFileListItemRef itemRef = (LSSharedFileListItemRef)[loginItemsArray objectAtIndex:i];
			//Resolve the item with URL
			if (LSSharedFileListItemResolve(itemRef, 0, (CFURLRef*) &url, NULL) == noErr)
			{
				NSString* urlPath = [(NSURL*) url path];
				if ([urlPath compare : appPath] == NSOrderedSame)
				{
					if (pRemove)
					{
						LSSharedFileListItemRemove(loginItems, itemRef);
					}

					return true;
				}
			}
			else
			{
				const CFStringRef stringRef = LSSharedFileListItemCopyDisplayName(itemRef);
				if (stringRef)
				{
					const QString displayName = QString::fromCFString(stringRef);
					if (displayName.startsWith(QCoreApplication::applicationName()))
					{
						LSSharedFileListItemRemove(loginItems, itemRef);
						qDebug() << "Removed the unresolvable application with diplay name" << displayName << "from OSX login items.";
					}
				}
			}
		}
		[loginItemsArray release];
	}

	return false;
}


#endif


bool GeneralSettings::readSettingsAutoStart()
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		return Test_AutoStart;
	}
#endif

#if defined(Q_OS_WIN32)
	QSettings windowsBootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	return windowsBootUpSettings.contains(QCoreApplication::applicationName());

#elif defined(Q_OS_OSX)
	return checkAndRemoveAutoStartMacOS(false);

#else
	qDebug() << "Autostart not supported on this system";
	return false;

#endif
}


void GeneralSettings::writeSettingsAutoStart(bool pAutoStart)
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		Test_AutoStart = pAutoStart;
		return;
	}
#endif


#ifdef Q_OS_WIN32
	QSettings windowsBootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

	if (pAutoStart)
	{
		QString applicationFilePath = QCoreApplication::applicationFilePath();
		applicationFilePath.replace(QStringLiteral("/"), QString("\\"));
		windowsBootUpSettings.setValue(QCoreApplication::applicationName(), "\"" + applicationFilePath + "\"");
	}
	else
	{
		windowsBootUpSettings.remove(QCoreApplication::applicationName());
	}
#elif defined(Q_OS_OSX)

	pAutoStart ? addAutoStartMacOS() : checkAndRemoveAutoStartMacOS();

#else
	qDebug() << "Autostart not supported on this system";
	Q_UNUSED(pAutoStart)
#endif
}


bool GeneralSettings::isAutoCloseWindowAfterAuthentication() const
{
	return mAutoCloseWindowAfterAuthentication;
}


void GeneralSettings::setAutoCloseWindowAfterAuthentication(bool pAutoClose)
{
	if (pAutoClose != mAutoCloseWindowAfterAuthentication)
	{
		mAutoCloseWindowAfterAuthentication = pAutoClose;
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isAutoUpdateCheck() const
{
	return mAutoUpdateCheck;
}


void GeneralSettings::setAutoUpdateCheck(bool pAutoCheck)
{
	if (pAutoCheck != mAutoUpdateCheck)
	{
		mAutoUpdateCheck = pAutoCheck;
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isAutoStart() const
{
	return mAutoStart;
}


void GeneralSettings::setAutoStart(bool pAutoStart)
{
	if (pAutoStart != mAutoStart)
	{
		mAutoStart.waitForFinished();
		mAutoStart = QtConcurrent::run([pAutoStart] {
			return pAutoStart;
		});

		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isUseScreenKeyboard() const
{
	return mUseScreenKeyboard;
}


void GeneralSettings::setUseScreenKeyboard(bool pKeylessPassword)
{
	if (pKeylessPassword != mUseScreenKeyboard)
	{
		mUseScreenKeyboard = pKeylessPassword;
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isShowSetupAssistant() const
{
	return mShowSetupAssistant;
}


void GeneralSettings::setShowSetupAssistant(bool pShowSetupAssistant)
{
	if (pShowSetupAssistant != mShowSetupAssistant)
	{
		mShowSetupAssistant = pShowSetupAssistant;
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isRemindUserToClose() const
{
	return mRemindUserToClose;
}


void GeneralSettings::setRemindUserToClose(bool pRemindUser)
{
	if (pRemindUser != mRemindUserToClose)
	{
		mRemindUserToClose = pRemindUser;
		Q_EMIT fireSettingsChanged();
	}
}


const QString& GeneralSettings::getPersistentSettingsVersion() const
{
	return mPersistentSettingsVersion;
}


bool GeneralSettings::isTransportPinReminder() const
{
	return mTransportPinReminder;
}


void GeneralSettings::setTransportPinReminder(bool pTransportPinReminder)
{
	if (pTransportPinReminder != mTransportPinReminder)
	{
		mTransportPinReminder = pTransportPinReminder;
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isDeveloperMode() const
{
	return mDeveloperMode;
}


void GeneralSettings::setDeveloperMode(bool pEnabled)
{
	if (pEnabled != mDeveloperMode)
	{
		mDeveloperMode = pEnabled;
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::useSelfauthenticationTestUri() const
{
	return mSelfauthenticationTestUri;
}


void GeneralSettings::setUseSelfauthenticationTestUri(bool pUse)
{
	if (pUse != mSelfauthenticationTestUri)
	{
		mSelfauthenticationTestUri = pUse;
		Q_EMIT fireSettingsChanged();
	}
}
