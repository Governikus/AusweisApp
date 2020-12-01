/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/DiagnosisContext.h"
#include "controller/DiagnosisController.h"
#include "DiagnosisModel.h"
#include "Env.h"

#include <QAbstractListModel>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QUrl>


namespace governikus
{

class SelfDiagnosisModel
	: public QObject
{
	Q_OBJECT
	friend class Env;

	Q_PROPERTY(QAbstractListModel * sectionsModel READ getSectionsModel CONSTANT)
	Q_PROPERTY(bool running READ isRunning NOTIFY fireRunningChanged)

	private:
		QSharedPointer<DiagnosisContext> mDiagnosisContext;
		DiagnosisModel mDiagnosisModel;
		QScopedPointer<DiagnosisController> mDiagnosisController;

		SelfDiagnosisModel();
		virtual ~SelfDiagnosisModel() override = default;

		bool isRunning() const;

	public:
		QAbstractListModel* getSectionsModel();
		Q_INVOKABLE QAbstractListModel* getSectionContentModel(const QString& pSection);
		Q_INVOKABLE void startController();
		Q_INVOKABLE void stopController();
		Q_INVOKABLE void saveToFile(const QUrl& pFilename) const;
		Q_INVOKABLE QString getCreationTimeString() const;

	Q_SIGNALS:
		void fireSectionContentModelChanged();
		void fireCurrentSectionChanged();
		void fireRunningChanged();
};

} // namespace governikus
