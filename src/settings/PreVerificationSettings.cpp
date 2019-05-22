/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "PreVerificationSettings.h"

using namespace governikus;

namespace
{
SETTINGS_NAME(SETTINGS_GROUP_NAME_PREVERIFICATION, "preverification")
SETTINGS_NAME(SETTINGS_NAME_ENABLED, "enabled")
SETTINGS_NAME(SETTINGS_NAME_LINKCERTIFICATES, "linkcertificates")
SETTINGS_NAME(SETTINGS_NAME_LINKCERTIFICATE, "linkcertificate")
} // namespace

PreVerificationSettings::PreVerificationSettings()
	: AbstractSettings()
	, mStore(getStore())
{
	mStore->beginGroup(SETTINGS_GROUP_NAME_PREVERIFICATION());
}


void PreVerificationSettings::updateLinkCertificates(const QByteArrayList& pLinkCertificates)
{
	mStore->beginGroup(SETTINGS_NAME_LINKCERTIFICATES());
	mStore->remove(QString());
	mStore->endGroup();

	mStore->beginWriteArray(SETTINGS_NAME_LINKCERTIFICATES());
	for (int i = 0; i < pLinkCertificates.size(); ++i)
	{
		mStore->setArrayIndex(i);
		mStore->setValue(SETTINGS_NAME_LINKCERTIFICATE(), pLinkCertificates.at(i));
	}
	mStore->endArray();
}


PreVerificationSettings::~PreVerificationSettings()
{
}


void PreVerificationSettings::save()
{
	mStore->sync();
}


bool PreVerificationSettings::isEnabled() const
{
	return mStore->value(SETTINGS_NAME_ENABLED(), true).toBool();
}


void PreVerificationSettings::setEnabled(bool pEnabled)
{
	mStore->setValue(SETTINGS_NAME_ENABLED(), pEnabled);
}


QByteArrayList PreVerificationSettings::getLinkCertificates() const
{
	const int itemCount = mStore->beginReadArray(SETTINGS_NAME_LINKCERTIFICATES());

	QByteArrayList linkCertificates;
	linkCertificates.reserve(itemCount);
	for (int i = 0; i < itemCount; ++i)
	{
		mStore->setArrayIndex(i);
		linkCertificates += mStore->value(SETTINGS_NAME_LINKCERTIFICATE()).toByteArray();
	}

	mStore->endArray();
	return linkCertificates;
}


void PreVerificationSettings::removeLinkCertificate(const QByteArray& pCert)
{
	auto linkCertificates = getLinkCertificates();
	if (linkCertificates.removeAll(pCert) > 0)
	{
		updateLinkCertificates(linkCertificates);
	}
}


void PreVerificationSettings::addLinkCertificate(const QByteArray& pCert)
{
	auto linkCertificates = getLinkCertificates();
	if (!linkCertificates.contains(pCert))
	{
		linkCertificates += pCert;
		updateLinkCertificates(linkCertificates);
	}
}
