/*!
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformationModel.h"

#include "AppSettings.h"
#include "AppUpdater.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

ReleaseInformationModel::ReleaseInformationModel()
	: QObject()
	, mReleaseInformationCurrent(Env::create<ReleaseInformation*>())
	, mFallbackModel(new FormattedTextModel(QStringList(tr("No release information present, make sure you are connected to the internet."))))
{
	connect(mReleaseInformationCurrent.data(), &ReleaseInformation::fireInformationChanged, this, &ReleaseInformationModel::onCurrentChanged);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppcastCheckFinished, this, &ReleaseInformationModel::onAppcastCheckFinished);
	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &ReleaseInformationModel::onLanguageChanged);

	onCurrentChanged();
}


QSharedPointer<FormattedTextModel> ReleaseInformationModel::createModel(const ReleaseInformation& pInformation) const
{
	QStringList fileList;

	fileList << pInformation.pathReleaseNotes();
	fileList << pInformation.pathAnnouncements();
	fileList << pInformation.pathIssues();

	const int emptyFilepathCount = fileList.removeAll(QString());
	if (emptyFilepathCount > 0)
	{
		qCWarning(qml) << "Missing" << emptyFilepathCount << "release information file(s)";
	}

	if (fileList.isEmpty())
	{
		qCWarning(qml) << "No release information files for version" << pInformation.versionNumber().getVersionNumber() << "found";
		return mFallbackModel;
	}

	const QSharedPointer<FormattedTextModel> textModel(new FormattedTextModel());
	if (!textModel->loadSeveral(fileList))
	{
		qCWarning(qml) << "Failed to update text model for version" << pInformation.versionNumber().getVersionNumber();
		return mFallbackModel;

	}
	return textModel;
}


void ReleaseInformationModel::onLanguageChanged()
{
	mReleaseInformationCurrent.reset(new ReleaseInformation());
	if (mReleaseInformationCurrent->requiresInitialUpdate())
	{
		mReleaseInformationCurrent->update();
	}
	else
	{
		onCurrentChanged();
	}

	if (mReleaseInformationUpdate)
	{
		setUpdateVersion(mReleaseInformationUpdate->versionNumber());
	}
}


void ReleaseInformationModel::onCurrentChanged()
{
	mModelCurrent = createModel(*mReleaseInformationCurrent);
	Q_EMIT fireCurrentInformationChanged();
}


void ReleaseInformationModel::onUpdateChanged()
{
	if (mReleaseInformationUpdate)
	{
		mModelUpdate = createModel(*mReleaseInformationUpdate);
		Q_EMIT fireUpdateInformationChanged();
	}
}


void ReleaseInformationModel::onAppcastCheckFinished(bool pUpdateAvailable, const GlobalStatus& pStatus)
{
	if (!pUpdateAvailable || pStatus.isError())
	{
		return;
	}

	const auto& updateData = Env::getSingleton<AppUpdater>()->getUpdateData();
	setUpdateVersion(VersionNumber(updateData.getVersion()));
}


QSharedPointer<FormattedTextModel> ReleaseInformationModel::getCurrentRelease() const
{
	return mModelCurrent ? mModelCurrent : mFallbackModel;
}


QSharedPointer<FormattedTextModel> ReleaseInformationModel::getUpdateRelease() const
{
	return mModelUpdate ? mModelUpdate : mFallbackModel;
}


void ReleaseInformationModel::update()
{
	mReleaseInformationCurrent->update();
	if (mReleaseInformationUpdate)
	{
		mReleaseInformationUpdate->update();
	}
}


bool ReleaseInformationModel::requiresInitialUpdate() const
{
	return mReleaseInformationCurrent->requiresInitialUpdate();
}


void ReleaseInformationModel::setUpdateVersion(const VersionNumber& pVersion)
{
	qCInfo(qml) << "Loading update release information for" << pVersion.getVersionNumber();

	if (mReleaseInformationUpdate)
	{
		mReleaseInformationUpdate->disconnect(this);
	}

	mReleaseInformationUpdate.reset(Env::create<ReleaseInformation*>(pVersion, true));
	connect(mReleaseInformationUpdate.data(), &ReleaseInformation::fireInformationChanged, this, &ReleaseInformationModel::onUpdateChanged);

	if (mReleaseInformationUpdate->requiresInitialUpdate())
	{
		mReleaseInformationUpdate->update();
	}
	else
	{
		onUpdateChanged();
	}
}


bool ReleaseInformationModel::allowRetry() const
{
	Q_ASSERT(!mFallbackModel.isNull());
	return mModelCurrent.isNull() || mModelCurrent == mFallbackModel;
}
