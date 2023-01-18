/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformationConfiguration.h"

#include "Env.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)

ReleaseInformationConfiguration::ReleaseInformationConfiguration()
	: QObject()
	, mReleaseInformationCurrent(Env::create<ReleaseInformation*>())
	, mReleaseInformationUpdate()
{
	connect(mReleaseInformationCurrent.data(), &ReleaseInformation::fireInformationChanged, this, &ReleaseInformationConfiguration::fireCurrentInformationChanged);
}


void ReleaseInformationConfiguration::update()
{
	mReleaseInformationCurrent->update();
	if (mReleaseInformationUpdate)
	{
		mReleaseInformationUpdate->update();
	}
}


void ReleaseInformationConfiguration::reload()
{
	setCurrentInformation(Env::create<ReleaseInformation*>());

	if (mReleaseInformationUpdate)
	{
		setUpdateVersion(mReleaseInformationUpdate->versionNumber());
	}
}


void ReleaseInformationConfiguration::updateIfNeeded()
{
	if (mReleaseInformationCurrent->requiresInitialUpdate())
	{
		update();
	}
}


void ReleaseInformationConfiguration::setUpdateVersion(const VersionNumber& pVersion)
{
	qCInfo(::update) << "Loading update release information for" << pVersion.getVersionNumber();

	setUpdateInformation(Env::create<ReleaseInformation*>(pVersion, true));
}


QSharedPointer<const ReleaseInformation> ReleaseInformationConfiguration::getCurrentInformation() const
{
	return mReleaseInformationCurrent;
}


QSharedPointer<const ReleaseInformation> ReleaseInformationConfiguration::getUpdateInformation() const
{
	return mReleaseInformationUpdate;
}


void ReleaseInformationConfiguration::setUpdateInformation(ReleaseInformation* pInfo)
{
	if (mReleaseInformationUpdate)
	{
		mReleaseInformationUpdate->disconnect(this);
	}

	mReleaseInformationUpdate.reset(pInfo);
	connect(mReleaseInformationUpdate.data(), &ReleaseInformation::fireInformationChanged, this, &ReleaseInformationConfiguration::fireUpdateInformationChanged);

	if (mReleaseInformationUpdate->requiresInitialUpdate())
	{
		mReleaseInformationUpdate->update();
	}
	else
	{
		Q_EMIT fireUpdateInformationChanged();
	}
}


void ReleaseInformationConfiguration::setCurrentInformation(ReleaseInformation* pInfo)
{
	if (mReleaseInformationCurrent)
	{
		mReleaseInformationCurrent->disconnect(this);
	}

	mReleaseInformationCurrent.reset(pInfo);
	connect(mReleaseInformationCurrent.data(), &ReleaseInformation::fireInformationChanged, this, &ReleaseInformationConfiguration::fireCurrentInformationChanged);

	if (mReleaseInformationCurrent->requiresInitialUpdate())
	{
		mReleaseInformationCurrent->update();
	}
	else
	{
		Q_EMIT fireCurrentInformationChanged();
	}
}
