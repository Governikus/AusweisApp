/*!
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ReleaseInformationModel.h"

#include "Env.h"
#include "ReleaseInformationConfiguration.h"

#include <QLoggingCategory>

#include <memory>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


FormattedTextModel* ReleaseInformationModel::createModel(const ReleaseInformation& pInformation)
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

	auto textModel = std::make_unique<FormattedTextModel>(this);
	if (textModel->loadSeveral(fileList))
	{
		return textModel.release();
	}

	qCWarning(qml) << "Failed to update text model for version" << pInformation.versionNumber().getVersionNumber();
	return mFallbackModel;
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


ReleaseInformationModel::ReleaseInformationModel()
	: QObject()
	, mFallbackModel(new FormattedTextModel(this, QStringList(tr("No release information present, make sure you are connected to the internet."))))
	, mModelCurrent(nullptr)
	, mModelUpdate(nullptr)
{
	const auto& releaseInformationConfig = Env::getSingleton<ReleaseInformationConfiguration>();
	connect(releaseInformationConfig, &ReleaseInformationConfiguration::fireCurrentInformationChanged, this, &ReleaseInformationModel::onCurrentChanged);
	connect(releaseInformationConfig, &ReleaseInformationConfiguration::fireUpdateInformationChanged, this, &ReleaseInformationModel::onUpdateChanged);

	onCurrentChanged();
	onUpdateChanged();
}


FormattedTextModel* ReleaseInformationModel::getCurrentRelease() const
{
	return mModelCurrent ? mModelCurrent : mFallbackModel;
}


FormattedTextModel* ReleaseInformationModel::getUpdateRelease() const
{
	return mModelUpdate ? mModelUpdate : mFallbackModel;
}


void ReleaseInformationModel::update()
{
	Env::getSingleton<ReleaseInformationConfiguration>()->update();
}


bool ReleaseInformationModel::allowRetry() const
{
	Q_ASSERT(mFallbackModel);
	return !mModelCurrent || mModelCurrent == mFallbackModel;
}


void ReleaseInformationModel::onTranslationChanged() const
{
	Env::getSingleton<ReleaseInformationConfiguration>()->reload();
}
