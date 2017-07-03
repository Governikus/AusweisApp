/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "RemoteReaderSettings.h"

using namespace governikus;

// names for settings groups/keys
const QLatin1String SETTINGS_GROUP_NAME_REMOTEREADER("remotereader");
const QLatin1String SETTINGS_NAME_DEVICE_NAME("serverName");
const QLatin1String SETTINGS_NAME_USE_ENCRYPTION("useEncryption");
const QLatin1String SETTINGS_NAME_PASSWORD("password");


RemoteReaderSettings::RemoteReaderSettings()
	: AbstractSettings()
	, mServerName(QLatin1String("RemoteReader"))
	, mUseEncryption(false)
	, mPassword("")
{
}


RemoteReaderSettings::~RemoteReaderSettings()
{
}


void RemoteReaderSettings::load()
{
	auto settings = getStore();

	settings->beginGroup(SETTINGS_GROUP_NAME_REMOTEREADER);
	mServerName = settings->value(SETTINGS_NAME_DEVICE_NAME, mServerName).toString();
	mUseEncryption = settings->value(SETTINGS_NAME_USE_ENCRYPTION, mUseEncryption).toBool();
	mPassword = settings->value(SETTINGS_NAME_PASSWORD, mPassword).toString();
	settings->endGroup();
}


bool RemoteReaderSettings::isUnsaved() const
{
	RemoteReaderSettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void RemoteReaderSettings::save()
{
	auto settings = getStore();

	settings->beginGroup(SETTINGS_GROUP_NAME_REMOTEREADER);
	settings->remove(QString());     // remove the whole group first
	settings->setValue(SETTINGS_NAME_DEVICE_NAME, mServerName);
	settings->setValue(SETTINGS_NAME_USE_ENCRYPTION, mUseEncryption);
	settings->setValue(SETTINGS_NAME_PASSWORD, mPassword);
	settings->endGroup();

	settings->sync();
}


QString RemoteReaderSettings::getServerName() const
{
	return mServerName;
}


void RemoteReaderSettings::setServerName(const QString& pName)
{
	mServerName = pName;
}


bool RemoteReaderSettings::useEncryption() const
{
	return mUseEncryption;
}


void RemoteReaderSettings::setEncryption(bool pEnabled)
{
	mUseEncryption = pEnabled;
}


QString RemoteReaderSettings::getPassword() const
{
	return mPassword;
}


void RemoteReaderSettings::setPassword(const QString& pPassword)
{
	mPassword = pPassword;
}
