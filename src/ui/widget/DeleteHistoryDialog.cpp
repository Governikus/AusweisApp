/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DeleteHistoryDialog.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>


using namespace governikus;


DeleteHistoryDialog::DeleteHistoryDialog(QWidget* pParent)
	: QDialog(pParent)
{
	setObjectName(QStringLiteral("DeleteHistoryDialog"));

	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Delete history"));
	setLayout(new QVBoxLayout(this));
	setMinimumSize(300, -1);

	layout()->setObjectName(QStringLiteral("dialogLayout"));

	QLabel* groupBoxLabel = new QLabel(this);
	groupBoxLabel->setText(tr("Delete history for this period:"));
	groupBoxLabel->setWordWrap(true);
	layout()->addWidget(groupBoxLabel);

	mComboBox = new QComboBox(this);
	layout()->addWidget(mComboBox);
	mComboBox->addItem(tr("Past hour"), static_cast<int>(TimePeriod::PAST_HOUR));
	mComboBox->addItem(tr("Past day"), static_cast<int>(TimePeriod::PAST_DAY));
	mComboBox->addItem(tr("Past week"), static_cast<int>(TimePeriod::PAST_WEEK));
	mComboBox->addItem(tr("Last four weeks"), static_cast<int>(TimePeriod::LAST_FOUR_WEEKS));
	mComboBox->addItem(tr("All history"), static_cast<int>(TimePeriod::ALL_HISTORY));

	auto* verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout()->addItem(verticalSpacer);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout()->addWidget(buttonBox);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


QRadioButton* DeleteHistoryDialog::createRadioButtonAndAppendToGroup(const QString& pText, TimePeriod pTimePeriod)
{
	auto* button = new QRadioButton(this);
	button->setText(pText);
	button->setProperty("TimePeriod", Enum<TimePeriod>::getValue(pTimePeriod));
	button->setObjectName(QStringLiteral("button%1").arg(QString::number(mRadioButtonGroup->buttons().count())));
	mRadioButtonGroup->addButton(button);
	return button;
}


TimePeriod DeleteHistoryDialog::getTimePeriod()
{
	return TimePeriod(mComboBox->currentData().toInt());
}
