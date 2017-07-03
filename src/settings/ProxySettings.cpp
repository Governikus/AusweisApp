/*!
 * ProxySettings.cpp
 *
 * \brief Contains the method definitions of the ProxySettings class.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProxySettings.h"

using namespace governikus;


const QLatin1String SETTINGS_GROUP_NAME_PROXY("proxy");
const QLatin1String SETTINGS_NAME_PROXY_TYPE("type");

const QLatin1String SETTINGS_PROXY_TYPE_NAME_NONE("none");
const QLatin1String SETTINGS_PROXY_TYPE_NAME_SYSTEM("system");


ProxySettings::ProxySettings()
	: AbstractSettings()
	, mProxyType(QNetworkProxy::DefaultProxy)
{
}


ProxySettings::~ProxySettings()
{
}


QNetworkProxy::ProxyType ProxySettings::getProxyType() const
{
	return mProxyType;
}


void ProxySettings::setProxyType(QNetworkProxy::ProxyType pProxyType)
{
	if (pProxyType != mProxyType)
	{
		mProxyType = pProxyType;
		Q_EMIT fireSettingsChanged();
	}
}


void ProxySettings::load()
{
	//use system proxy only
	return;

	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_PROXY);

	QString proxyTypeName = settings->value(SETTINGS_NAME_PROXY_TYPE, QString()).toString();
	if (proxyTypeName == SETTINGS_PROXY_TYPE_NAME_NONE)
	{
		mProxyType = QNetworkProxy::NoProxy;
	}
	else if (proxyTypeName == SETTINGS_PROXY_TYPE_NAME_SYSTEM)
	{
		mProxyType = QNetworkProxy::DefaultProxy;
	}
	settings->endGroup();
}


bool ProxySettings::isUnsaved() const
{
	ProxySettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void ProxySettings::save()
{
	//use system proxy only
	return;

	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_PROXY);
	settings->remove(QString()); // remove the whole group first

	auto proxyTypeName = mProxyType == QNetworkProxy::NoProxy ? SETTINGS_PROXY_TYPE_NAME_NONE : SETTINGS_PROXY_TYPE_NAME_SYSTEM;
	settings->setValue(SETTINGS_NAME_PROXY_TYPE, proxyTypeName);

	settings->endGroup();
	settings->sync();
}
