#pragma once

#include "EnumHelper.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QPointer>
#include <QRadioButton>

namespace governikus
{

defineEnumType(TimePeriod,
		PAST_HOUR,
		PAST_DAY,
		PAST_WEEK,
		LAST_FOUR_WEEKS,
		ALL_HISTORY
		)

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

} /* namespace governikus */
