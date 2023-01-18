/*!
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SelfDiagnosisModel.h"

#include "GeneralSettings.h"

#include <QFile>
#include <QQmlEngine>
#include <QSharedPointer>


using namespace governikus;


SelfDiagnosisModel::SelfDiagnosisModel()
	: QObject()
	, mDiagnosisContext(new DiagnosisContext())
	, mDiagnosisModel(mDiagnosisContext)
{
	connect(&mDiagnosisModel, &DiagnosisModel::fireRunningChanged, this, &SelfDiagnosisModel::fireRunningChanged);
}


bool SelfDiagnosisModel::isRunning() const
{
	return mDiagnosisModel.isRunning();
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


QString SelfDiagnosisModel::getCreationTime() const
{
	return mDiagnosisModel.getCreationTime();
}


void SelfDiagnosisModel::onTranslationChanged()
{
	mDiagnosisModel.reloadContent();
}


QAbstractListModel* SelfDiagnosisModel::getSectionsModel()
{
	return &mDiagnosisModel;
}


void SelfDiagnosisModel::startController()
{
	if (mDiagnosisController.isNull())
	{
		mDiagnosisController.reset(new DiagnosisController(mDiagnosisContext));
		mDiagnosisController->run();
	}
	mDiagnosisModel.reloadContent();
}


void SelfDiagnosisModel::stopController()
{
	mDiagnosisController.reset();
}
