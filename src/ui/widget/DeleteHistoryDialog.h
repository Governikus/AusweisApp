/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HistorySettings.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QPointer>
#include <QRadioButton>

namespace governikus
{

class DeleteHistoryDialog
	: public QDialog
{
	Q_OBJECT
	QPointer<QButtonGroup> mRadioButtonGroup;
	QPointer<QComboBox> mComboBox;

	QRadioButton* createRadioButtonAndAppendToGroup(const QString& pText, TimePeriod pTimePeriod);

	public:
		explicit DeleteHistoryDialog(QWidget* pParent = nullptr);
		TimePeriod getTimePeriod();
};

} // namespace governikus
