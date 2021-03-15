/*!
 * \brief Provides information about the App release to QML
 *
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "FormattedTextModel.h"
#include "GlobalStatus.h"
#include "ReleaseInformation.h"

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

class test_ReleaseInformationModel;

namespace governikus
{

class ReleaseInformationModel
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_ReleaseInformationModel;

	Q_PROPERTY(QSharedPointer<FormattedTextModel> currentRelease READ getCurrentRelease NOTIFY fireCurrentInformationChanged)
	Q_PROPERTY(QSharedPointer<FormattedTextModel> updateRelease READ getUpdateRelease NOTIFY fireUpdateInformationChanged)
	Q_PROPERTY(bool allowRetry READ allowRetry NOTIFY fireCurrentInformationChanged)

	private:
		QScopedPointer<ReleaseInformation> mReleaseInformationCurrent;
		QScopedPointer<ReleaseInformation> mReleaseInformationUpdate;
		QSharedPointer<FormattedTextModel> mFallbackModel;
		QSharedPointer<FormattedTextModel> mModelCurrent;
		QSharedPointer<FormattedTextModel> mModelUpdate;

		ReleaseInformationModel();
		QSharedPointer<FormattedTextModel> createModel(const ReleaseInformation& pInformation) const;

	private Q_SLOTS:
		void onLanguageChanged();
		void onCurrentChanged();
		void onUpdateChanged();
		void onAppcastCheckFinished(bool pUpdateAvailable, const GlobalStatus& pStatus);

	public:
		~ReleaseInformationModel() override = default;
		QSharedPointer<FormattedTextModel> getCurrentRelease() const;
		QSharedPointer<FormattedTextModel> getUpdateRelease() const;
		Q_INVOKABLE void update();
		bool requiresInitialUpdate() const;
		void setUpdateVersion(const VersionNumber& pVersion);
		bool allowRetry() const;

	Q_SIGNALS:
		void fireCurrentInformationChanged();
		void fireUpdateInformationChanged();
};

} // namespace governikus
