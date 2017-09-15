/*!
 * SelfInfoWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SelfInfoWidget.h"

#include "generic/ListItem.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"
#include "PdfCreator.h"
#include "SelfAuthenticationData.h"
#include "ui_SelfInfoWidget.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLoggingCategory>
#include <QPainter>
#include <QPushButton>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


SelfInfoWidget::SelfInfoWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::SelfInfoWidget())
	, mData(nullptr)
{
	mUi->setupUi(this);
	layout()->setMargin(20);
}


SelfInfoWidget::~SelfInfoWidget()
{
}


QString SelfInfoWidget::formatDateString(const QString& pIn)
{
	QDateTime dateTime = QDateTime::fromString(pIn, QStringLiteral("yyyy-MM-dd+hh:mm"));
	return dateTime.toString(tr("dd.MM.yyyy"));
}


void SelfInfoWidget::setInfo(SelfAuthenticationData* pData)
{
	mData = pData;

	//delete old data from ui
	QLayout* uiLayout = mUi->dataLayout;

	cleanLayout(uiLayout);
	cleanLayout(mUi->saveLayout);


	//fill layout with new data, see 18 Personalausweisgesetz (PAuswG)
	if (!mData->getValue(SelfAuthData::FamilyNames).isNull())
	{
		insertDataRow(tr("Family name:"), mData->getValue(SelfAuthData::FamilyNames));
	}
	if (!mData->getValue(SelfAuthData::BirthName).isNull())
	{
		insertDataRow(tr("Birth name:"), mData->getValue(SelfAuthData::BirthName));
	}
	if (!mData->getValue(SelfAuthData::GivenNames).isNull())
	{
		insertDataRow(tr("Given name(s):"), mData->getValue(SelfAuthData::GivenNames));
	}
	if (!mData->getValue(SelfAuthData::AcademicTitle).isNull())
	{
		insertDataRow(tr("Doctoral degree:"), mData->getValue(SelfAuthData::AcademicTitle));
	}
	if (!mData->getValue(SelfAuthData::DateOfBirth).isNull())
	{
		insertDataRow(tr("Date of birth:"), formatDateString(mData->getValue(SelfAuthData::DateOfBirth)));
	}
	if (!mData->getValue(SelfAuthData::PlaceOfBirth).isNull())
	{
		insertDataRow(tr("Place of birth:"), mData->getValue(SelfAuthData::PlaceOfBirth));
	}

	if (!mData->getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull())
	{
		insertDataRow(tr("Address:"), mData->getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo));
	}
	if (!mData->getValue(SelfAuthData::PlaceOfResidenceStreet).isNull())
	{
		insertDataRow(mData->getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull() ? tr("Address:") : QString(), mData->getValue(SelfAuthData::PlaceOfResidenceStreet));
	}
	if (!mData->getValue(SelfAuthData::PlaceOfResidenceZipCode).isNull() || !mData->getValue(SelfAuthData::PlaceOfResidenceCity).isNull())
	{
		insertDataRow(mData->getValue(SelfAuthData::PlaceOfResidenceStreet).isNull() ? tr("Address:") : QString(), mData->getValue(SelfAuthData::PlaceOfResidenceZipCode) + ' ' + mData->getValue(SelfAuthData::PlaceOfResidenceCity));

	}
	if (!mData->getValue(SelfAuthData::PlaceOfResidenceCountry).isNull())
	{
		insertDataRow(QString(), mData->getValue(SelfAuthData::PlaceOfResidenceCountry));

	}

	auto documentType = mData->getValue(SelfAuthData::DocumentType);
	if (!documentType.isNull())
	{
		insertDataRow(tr("Document type:"), documentType);
	}
	if (!mData->getValue(SelfAuthData::Nationality).isNull())
	{
		insertDataRow(tr("Nationality:"), mData->getValue(SelfAuthData::Nationality));
	}
	if (!mData->getValue(SelfAuthData::ArtisticName).isNull())
	{
		insertDataRow(tr("Religious / artistic name:"), mData->getValue(SelfAuthData::ArtisticName));
	}
	if (!mData->getValue(SelfAuthData::IssuingState).isNull())
	{
		insertDataRow(tr("Issuing country:"), mData->getValue(SelfAuthData::IssuingState));
	}

	// Show "Residence Permit" for eAT- and Test-Cards only
	// AR, AS, AF --> see TR-03127 (v1.16) chapter 3.2.3
	// TA --> Used by Test-Cards
	if (!mData->getValue(SelfAuthData::ResidencePermitI).isNull() && (
				documentType == QLatin1String("AR") ||
				documentType == QLatin1String("AS") ||
				documentType == QLatin1String("AF") ||
				documentType == QLatin1String("TA")))
	{
		insertDataRow(tr("Residence permit I:"), mData->getValue(SelfAuthData::ResidencePermitI));
	}

	QPushButton* exportButton = new QPushButton(tr("Save as PDF..."));
	exportButton->setAccessibleName(tr("save id card data as pdf"));

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


void SelfInfoWidget::insertDataRow(const QString& pLabel, const QString& pField)
{
	QLabel* tmpLabel = new QLabel(pLabel);
	tmpLabel->setFocusPolicy(Qt::TabFocus);
	tmpLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

	QLabel* tmpField = new QLabel(pField);
	tmpField->setFocusPolicy(Qt::TabFocus);
	tmpField->setWordWrap(true);

	mUi->dataLayout->insertRow(-1, tmpLabel, tmpField);
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
	if (mData != nullptr && mData->isValid())
	{
		QString pdfName = QFileDialog::getSaveFileName(this,
				QApplication::applicationName() + " - " + tr("Save"),
				QDir::currentPath(),
				tr("PDF Documents") + " (*.pdf)");

		if (pdfName.size() > 0)
		{
			PdfCreator pdf(pdfName);
			QString text;

			QString date = mData->getDateTime().toString(tr("MM/dd/yyyy"));
			QString time = mData->getDateTime().toString(tr("hh:mm AP"));
			pdf.setHeadline(tr("At %1 %2 the following data has been read out of your ID card:").arg(date, time));

			pdf.initTable(2, {180}, {tr("Entry"), tr("Content")});
			for (int i = 0; i < mUi->dataLayout->count(); ++i)
			{
				QLayoutItem* item = mUi->dataLayout->itemAt(i);
				if (!item->isEmpty())
				{
					QLabel* label = qobject_cast<QLabel*>(item->widget());

					if (i % 2 == 0)
					{
						text = label->text();
					}
					else
					{
						pdf.addTableRow({text, label->text()});
						pdf.toggleRowColor();
					}
				}
			}

			pdf.save();
			QDesktopServices::openUrl(pdf.getFileUrl());
		}
	}
}
