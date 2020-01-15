/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "SelfInfoWidget.h"

#include "ui_SelfInfoWidget.h"

#include "generic/ListItem.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"
#include "PdfExporter.h"

#include <QFileDialog>
#include <QLoggingCategory>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


SelfInfoWidget::SelfInfoWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::SelfInfoWidget())
	, mSelfAuthenticationData()
{
	mUi->setupUi(this);
	layout()->setMargin(20);
}


SelfInfoWidget::~SelfInfoWidget()
{
}


void SelfInfoWidget::setInfo(const SelfAuthenticationData& pData)
{
	mSelfAuthenticationData = pData;
	fillLayout();
}


void SelfInfoWidget::fillLayout()
{
	//delete old data from ui
	QLayout* uiLayout = mUi->dataLayout;
	cleanLayout(uiLayout);
	cleanLayout(mUi->saveLayout);

	const auto& orderedSelfData = mSelfAuthenticationData.getOrderedSelfData();
	for (const auto& entry : orderedSelfData)
	{
		QLabel* const tmpLabel = new QLabel(entry.first);
		if (!entry.first.isEmpty())
		{
			tmpLabel->setFocusPolicy(Qt::TabFocus);
		}
		tmpLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

		QLabel* const tmpField = new QLabel(entry.second);
		tmpField->setFocusPolicy(Qt::TabFocus);
		tmpField->setWordWrap(true);

		mUi->dataLayout->insertRow(-1, tmpLabel, tmpField);
	}

	QPushButton* exportButton = new QPushButton(tr("Save as PDF..."));
	exportButton->setAccessibleName(tr("save ID card data as pdf"));

	connect(exportButton, &QPushButton::clicked, this, &SelfInfoWidget::onPrintButtonClicked);

	mUi->saveLayout->addWidget(exportButton);
	mUi->saveLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}


void SelfInfoWidget::cleanLayout(QLayout* pLayout)
{
	while (QLayoutItem* child = pLayout->itemAt(0))
	{
		if (QWidget* childWidget = child->widget())
		{
			pLayout->removeWidget(childWidget);
			delete childWidget;
		}
		else
		{
			pLayout->removeItem(child);
			delete child;
		}
	}
}


void SelfInfoWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void SelfInfoWidget::onPrintButtonClicked()
{
	const auto& selfData = mSelfAuthenticationData.getOrderedSelfData();
	if (!selfData.isEmpty())
	{
		const auto& dataTime = mSelfAuthenticationData.getDateTime();
		QString filename = tr("AusweisApp2.Information.%1.pdf").arg(dataTime.toString(QStringLiteral("yyyy-MM-dd")));
		filename = QFileDialog::getSaveFileName(this,
				QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Save"),
				QDir::homePath() + QLatin1Char('/') + filename,
				tr("PDF Documents") + QStringLiteral(" (*.pdf)"));

		PdfExporter exporter(filename);
		exporter.exportSelfInfo(dataTime, selfData);
	}
}


void SelfInfoWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
		fillLayout();
	}
	QWidget::changeEvent(pEvent);
}
