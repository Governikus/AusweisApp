/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SurveyModel.h"

#include "Env.h"


Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;

SurveyModel::SurveyModel()
	: QAbstractListModel()
	, mData()
{
	connect(Env::getSingleton<Survey>(), &Survey::fireSurveyDataChanged, this, &SurveyModel::onSurveyDataChanged);
	onSurveyDataChanged();
}


void SurveyModel::onSurveyDataChanged()
{
	beginResetModel();
	mData = Env::getSingleton<Survey>()->getSurveyData();
	endResetModel();
}


int SurveyModel::rowCount(const QModelIndex&) const
{
	return static_cast<int>(mData.size());
}


QVariant SurveyModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		const auto& [title, value] = mData[pIndex.row()];
		if (pRole == TITLE)
		{
			return title;
		}
		if (pRole == VALUE)
		{
			return value;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> SurveyModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(TITLE, "title");
	roles.insert(VALUE, "value");
	return roles;
}


bool SurveyModel::askForDeviceSurvey() const
{
	return Env::getSingleton<Survey>()->askForDeviceSurvey();
}


void SurveyModel::setDeviceSurveyPending(bool pValue) const
{
	return Env::getSingleton<Survey>()->setDeviceSurveyPending(pValue);
}
