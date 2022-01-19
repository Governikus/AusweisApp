/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformationModel.h"

#include "AppSettings.h"
#include "ReleaseInformationConfiguration.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

ReleaseInformationModel::ReleaseInformationModel()
	: QObject()
	, mFallbackModel(new FormattedTextModel(QStringList(tr("No release information present, make sure you are connected to the internet."))))
{
	const auto& releaseInformationConfig = Env::getSingleton<ReleaseInformationConfiguration>();
	connect(releaseInformationConfig, &ReleaseInformationConfiguration::fireCurrentInformationChanged, this, &ReleaseInformationModel::onCurrentChanged);
	connect(releaseInformationConfig, &ReleaseInformationConfiguration::fireUpdateInformationChanged, this, &ReleaseInformationModel::onUpdateChanged);

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &ReleaseInformationModel::onTranslationChanged);

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


void ReleaseInformationModel::onCurrentChanged()
{
	const auto& currentInformation = Env::getSingleton<ReleaseInformationConfiguration>()->getCurrentInformation();
	if (currentInformation)
	{
		mModelCurrent = createModel(*currentInformation);
		Q_EMIT fireCurrentInformationChanged();
	}
}


void ReleaseInformationModel::onUpdateChanged()
{
	const auto& updateInformation = Env::getSingleton<ReleaseInformationConfiguration>()->getUpdateInformation();
	if (updateInformation)
	{
		mModelUpdate = createModel(*updateInformation);
		Q_EMIT fireUpdateInformationChanged();
	}
}


void ReleaseInformationModel::onTranslationChanged()
{
	Env::getSingleton<ReleaseInformationConfiguration>()->reload();
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
	Env::getSingleton<ReleaseInformationConfiguration>()->update();
}


bool ReleaseInformationModel::allowRetry() const
{
	Q_ASSERT(!mFallbackModel.isNull());
	return mModelCurrent.isNull() || mModelCurrent == mFallbackModel;
}
