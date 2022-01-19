/*!
 * \brief Provides information about the App release to QML
 *
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
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
		QSharedPointer<FormattedTextModel> mFallbackModel;
		QSharedPointer<FormattedTextModel> mModelCurrent;
		QSharedPointer<FormattedTextModel> mModelUpdate;

		ReleaseInformationModel();
		QSharedPointer<FormattedTextModel> createModel(const ReleaseInformation& pInformation) const;

	private Q_SLOTS:
		void onCurrentChanged();
		void onUpdateChanged();

	public:
		~ReleaseInformationModel() override = default;
		QSharedPointer<FormattedTextModel> getCurrentRelease() const;
		QSharedPointer<FormattedTextModel> getUpdateRelease() const;
		Q_INVOKABLE void update();
		bool allowRetry() const;

	public Q_SLOTS:
		void onTranslationChanged();

	Q_SIGNALS:
		void fireCurrentInformationChanged();
		void fireUpdateInformationChanged();
};

} // namespace governikus
