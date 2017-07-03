/*!
 * SelfInfoWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SelfAuthenticationData.h"

#include "SelfInfoWidget.h"
#include "ui_SelfInfoWidget.h"

#include "generic/ListItem.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLoggingCategory>
#include <QPainter>
#include <QPushButton>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>


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
		QString htmlContent, company_header, company_footer;
		QPrinter printer;
		QRect printer_rect(printer.pageRect());

		QString pdfName = QFileDialog::getSaveFileName(this,
				QApplication::applicationName() + " - " + tr("Save"),
				QDir::currentPath(),
				tr("PDF Documents") + " (*.pdf)");

		if (pdfName.size() > 0)
		{
			printer.setOutputFileName(pdfName);

			//Header
			QString date = mData->getDateTime().toString(QStringLiteral("dd.MM.yyyy"));
			QString time = mData->getDateTime().toString(QStringLiteral("hh:mm"));

			company_header.append("<br><br><table cellspacing='5' cellpadding='5' width='100%'>");
			company_header.append("<tr>");
			company_header.append(tr("<td valign='middle'><h1>ID card info - ") + QApplication::applicationName() + tr("</h1><h3>AusweisApp2 is a product of Governikus GmbH & Co. KG - on behalf of the Bundesministerium des Innern (Federal Ministry of the Interior).</h3></td>"));
			company_header.append("<th rowspan='2'><p align='center'><img src=':/images/information.png'></p></th>");
			company_header.append("</tr>");
			company_header.append("<tr>");

			company_header.append(tr("<td valign='middle'><br><h3>At %1 %2 the following data has been read out of your ID card:").arg(date, time) + "</h3></td>");
			company_header.append("</tr>");
			company_header.append("</table>");

			company_footer.append(tr("<h3>For further information, please see <a href='https://www.ausweisapp.bund.de/'>https://www.ausweisapp.bund.de/</a></h3>"));

			//Setting up the header and calculating the header size
			QTextDocument* document_header = new QTextDocument(this);
			document_header->setPageSize(printer_rect.size());
			document_header->setHtml(company_header);
			QSizeF header_size = document_header->size();

			//Setting up the footer and calculating the footer size
			QTextDocument* document_footer = new QTextDocument(this);
			document_footer->setPageSize(printer_rect.size());
			document_footer->setHtml(company_footer);
			QSizeF footer_size = document_footer->size();

			//Calculating the main document size for one page
			QSizeF center_size(printer_rect.width(),
					(printer.pageRect().height() - header_size.toSize().height() - footer_size.toSize().height()));

			htmlContent.append("<table cellspacing='3' cellpadding='5'>");

			for (int i = 0; i < mUi->dataLayout->count(); ++i)
			{
				QLayoutItem* item = mUi->dataLayout->itemAt(i);
				if (!item->isEmpty())
				{
					if (i % 2 == 0)
					{
						htmlContent.append("<tr><td>");
					}
					else
					{
						htmlContent.append("<td>");
					}

					QLabel* test = qobject_cast<QLabel*>(item->widget());
					htmlContent.append("" + test->text());

					if (i % 2 == 0)
					{
						htmlContent.append("</td>");
					}
					else
					{
						htmlContent.append("</td></tr>");
					}
				}
			}

			htmlContent.append("</table>");

			//Insert HTML in main document
			QTextDocument* main_doc = new QTextDocument(this);
			main_doc->setHtml(htmlContent);
			main_doc->setPageSize(center_size);

			//Setting up the rectangles for each section.
			QRect headerRect = QRect(QPoint(0, 0), document_header->size().toSize());
			QRect footerRect = QRect(QPoint(0, 0), document_footer->size().toSize());
			QRect contentRect = QRect(QPoint(0, 0), main_doc->size().toSize()); // Main content rectangle.
			QRect currentRect = QRect(QPoint(0, 0), center_size.toSize()); // Current main content rectangle.

			QPainter painter(&printer);

			while (currentRect.intersects(contentRect))
			{ //Loop if the current content rectangle intersects with the main content rectangle.
				//Resetting the painter matrix co-ordinate system.
				painter.resetMatrix();
				//Applying negative translation of painter co-ordinate system by current main content rectangle top y coordinate.
				painter.translate(0, -currentRect.y());
				//Applying positive translation of painter co-ordinate system by header hight.
				painter.translate(0, headerRect.height());
				//Drawing the center content for current page.
				main_doc->drawContents(&painter, currentRect);
				//Resetting the painter matrix co ordinate system.
				painter.resetMatrix();
				//Drawing the header on the top of the page
				document_header->drawContents(&painter, headerRect);
				//Applying positive translation of painter co-ordinate system to draw the footer
				painter.translate(0, headerRect.height());
				painter.translate(0, center_size.height());
				document_footer->drawContents(&painter, footerRect);

				//Translating the current rectangle to the area to be printed for the next page
				currentRect.translate(0, currentRect.height());
				//Inserting a new page if there is till area left to be printed
				if (currentRect.intersects(contentRect))
				{
					printer.newPage();
				}
			}
			QDesktopServices::openUrl(QUrl(pdfName));
		}
	}
}
