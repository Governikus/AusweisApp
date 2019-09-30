/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include "SelfDiagnosisModel.h"

#include "AppSettings.h"
#include "GeneralSettings.h"
#include "SingletonHelper.h"

#include <QFile>
#include <QQmlEngine>
#include <QSharedPointer>


using namespace governikus;

defineSingleton(SelfDiagnosisModel)


SelfDiagnosisModel::SelfDiagnosisModel(QObject* pParent)
	: QObject(pParent)
	, mDiagnosisContext(new DiagnosisContext)
	, mDiagnosisModel(mDiagnosisContext)
{
	const GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &SelfDiagnosisModel::fireSectionContentModelChanged);
}


void SelfDiagnosisModel::saveToFile(const QUrl& pFilename) const
{
	QFile file(pFilename.toLocalFile());
	if (file.open(QIODevice::WriteOnly))
	{
		QString diagnosisLog = mDiagnosisModel.getAsPlaintext();
		file.write(diagnosisLog.toUtf8());
	}
}


QString SelfDiagnosisModel::getCreationTimeString() const
{
	return mDiagnosisModel.getCreationTimeString();
}


SelfDiagnosisModel& SelfDiagnosisModel::getInstance()
{
	return *Instance;
}


QAbstractListModel* SelfDiagnosisModel::getSectionsModel()
{
	return &mDiagnosisModel;
}


QAbstractListModel* SelfDiagnosisModel::getSectionContentModel(const QString& pSection)
{
	const auto model = mDiagnosisModel.getSectionContent(pSection);
	QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
	return model;
}


void SelfDiagnosisModel::startController()
{
	if (mDiagnosisController.isNull())
	{
		mDiagnosisController.reset(new DiagnosisController(mDiagnosisContext));
		mDiagnosisController->run();
	}
}


void SelfDiagnosisModel::stopController()
{
	mDiagnosisController.reset();
}
