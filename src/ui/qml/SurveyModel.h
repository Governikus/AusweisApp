/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SingletonCreator.h"
#include "Survey.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

namespace governikus
{

class SurveyModel
	: public QAbstractListModel
	, public SingletonCreator<SurveyModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;

	private:
		Survey::SurveyData mData;

		enum UserRoles
		{
			TITLE = Qt::UserRole + 1,
			VALUE
		};

		SurveyModel();
		~SurveyModel() override = default;

	private Q_SLOTS:
		void onSurveyDataChanged();

	public:
		[[nodiscard]] int rowCount(const QModelIndex& = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] Q_INVOKABLE bool askForDeviceSurvey() const;
		Q_INVOKABLE void setDeviceSurveyPending(bool pValue) const;
};

} // namespace governikus
