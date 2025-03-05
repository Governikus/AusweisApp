/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "FormattedTextModel.h"
#include "ReleaseInformation.h"

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QtQml/qqmlregistration.h>


class test_ReleaseInformationModel;

namespace governikus
{

class ReleaseInformationModel
	: public QObject
{
	Q_OBJECT
	QML_ELEMENT

	friend class ::test_ReleaseInformationModel;

	Q_PROPERTY(governikus::FormattedTextModel * currentRelease READ getCurrentRelease NOTIFY fireCurrentInformationChanged)
	Q_PROPERTY(governikus::FormattedTextModel * updateRelease READ getUpdateRelease NOTIFY fireUpdateInformationChanged)
	Q_PROPERTY(bool allowRetry READ allowRetry NOTIFY fireCurrentInformationChanged)

	private:
		FormattedTextModel* mFallbackModel;
		FormattedTextModel* mModelCurrent;
		FormattedTextModel* mModelUpdate;

		FormattedTextModel* createModel(const ReleaseInformation& pInformation);

	private Q_SLOTS:
		void onCurrentChanged();
		void onUpdateChanged();

	public:
		ReleaseInformationModel();
		~ReleaseInformationModel() override = default;
		[[nodiscard]] FormattedTextModel* getCurrentRelease() const;
		[[nodiscard]] FormattedTextModel* getUpdateRelease() const;
		Q_INVOKABLE void update() const;
		[[nodiscard]] bool allowRetry() const;

	public Q_SLOTS:
		void onTranslationChanged() const;

	Q_SIGNALS:
		void fireCurrentInformationChanged();
		void fireUpdateInformationChanged();
};

} // namespace governikus
