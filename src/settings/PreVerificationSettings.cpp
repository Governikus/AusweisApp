/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PreVerificationSettings.h"

using namespace governikus;

// names for settings groups/keys
const QLatin1String SETTINGS_GROUP_NAME_PREVERIFICATION("preverification");
const QLatin1String SETTINGS_NAME_ENABLED("enabled");
const QLatin1String SETTINGS_NAME_LINKCERTIFICATES("linkcertificates");
const QLatin1String SETTINGS_NAME_LINKCERTIFICATE("linkcertificate");


PreVerificationSettings::PreVerificationSettings()
	: AbstractSettings()
	, mLinkCertificates()
	, mEnabled(true)
{
}


PreVerificationSettings::~PreVerificationSettings()
{
}


void PreVerificationSettings::load()
{
	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_PREVERIFICATION);
	mEnabled = settings->value(SETTINGS_NAME_ENABLED, mEnabled).toBool();
	const int itemCount = settings->beginReadArray(SETTINGS_NAME_LINKCERTIFICATES);

	QByteArrayList linkCertificates;
	linkCertificates.reserve(itemCount);
	for (int i = 0; i < itemCount; ++i)
	{
		settings->setArrayIndex(i);
		linkCertificates += settings->value(SETTINGS_NAME_LINKCERTIFICATE).toByteArray();
	}
	mLinkCertificates = linkCertificates;

	settings->endArray();
	settings->endGroup();
}


bool PreVerificationSettings::isUnsaved() const
{
	PreVerificationSettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void PreVerificationSettings::save()
{
	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_PREVERIFICATION);
	settings->remove(QString());     // remove the whole group first
	settings->setValue(SETTINGS_NAME_ENABLED, mEnabled);
	settings->beginWriteArray(SETTINGS_NAME_LINKCERTIFICATES);

	for (int i = 0; i < mLinkCertificates.size(); ++i)
	{
		settings->setArrayIndex(i);
		settings->setValue(SETTINGS_NAME_LINKCERTIFICATE, mLinkCertificates.at(i));
	}

	settings->endArray();
	settings->endGroup();

	settings->sync();
}


bool PreVerificationSettings::isEnabled() const
{
	return mEnabled;
}


void PreVerificationSettings::setEnabled(bool pEnabled)
{
	mEnabled = pEnabled;
}


const QByteArrayList& PreVerificationSettings::getLinkCertificates() const
{
	return mLinkCertificates;
}


bool PreVerificationSettings::removeLinkCertificate(const QByteArray& pCert)
{
	bool changed = mLinkCertificates.removeAll(pCert);
	if (changed)
	{
		Q_EMIT fireSettingsChanged();
	}
	return changed;
}


bool PreVerificationSettings::addLinkCertificate(const QByteArray& pCert)
{
	if (mLinkCertificates.contains(pCert))
	{
		return false;
	}
	mLinkCertificates += pCert;
	Q_EMIT fireSettingsChanged();
	return true;
}
