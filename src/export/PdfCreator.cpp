/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "PdfCreator.h"

#include "AppSettings.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QPainter>
#include <QStringBuilder>
#include <QSvgRenderer>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>


using namespace governikus;


bool PdfExport::exportHistory(const QString& pFileName)
{
	if (pFileName.isEmpty())
	{
		return false;
	}


	PdfCreator pdf(pFileName);

	QDateTime dateTime = QDateTime::currentDateTime();
	QString date = dateTime.toString(tr("MM/dd/yyyy"));
	QString time = dateTime.toString(tr("hh:mm AP"));
	pdf.setHeadline(tr("At %1 %2 the following data were saved:").arg(date, time));

	pdf.initTable(3, {180, 80}, {tr("Date"), tr("Details")});
	auto history = AppSettings::getInstance().getHistorySettings().getHistoryEntries();
	for (int i = 0; i < history.size(); ++i)
	{
		pdf.addTableRow({history.at(i).getDateTime().toString(tr("MM/dd/yyyy hh:mm AP")), tr("Provider:"), history.at(i).getSubjectName()});
		pdf.addTableRow({QString(), tr("Purpose:"), history.at(i).getPurpose()});
		pdf.addTableRow({QString(), tr("Data:"), history.at(i).getRequestedData()});
		pdf.toggleRowColor();
	}

	pdf.save();
	return QDesktopServices::openUrl(pdf.getFileUrl());
}


void PdfCreator::closeTable()
{
	mContent += QLatin1String("</table>");
	mColumnCount = 0;
}


PdfCreator::PdfCreator(const QString& pFilename)
	: mFilename(pFilename)
	, mColoredRow(false)
	, mColumnCount(0)
	, mHeadline()
	, mContent()
{
	if (!mFilename.endsWith(QLatin1String(".pdf"), Qt::CaseInsensitive))
	{
		mFilename += QLatin1String(".pdf");
	}
}


PdfCreator::~PdfCreator()
{

}


QUrl PdfCreator::getFileUrl()
{
	return QUrl(mFilename);
}


void PdfCreator::setHeadline(const QString& pHeadline)
{
	mHeadline = pHeadline;
}


void PdfCreator::initTable(int pColumnCount, const QList<int>& pWidth, const QStringList& pValues)
{
	if (!mColumnCount)
	{
		mColumnCount = pColumnCount;
		mContent += QLatin1String("<table width='100%' cellspacing='0' cellpadding='6'>"\
								  "<tr>");
		for (int i = 0; i < mColumnCount; ++i)
		{
			const auto& width = i < pWidth.size() ? QStringLiteral(" width='%1'").arg(pWidth[i]) : QString();
			const auto& value = i < pValues.size() && !pValues[i].isEmpty() ? pValues[i] : QLatin1String("&nbsp;");
			mContent += QStringLiteral("<th align='left'%1>"
									   "%2"
									   "</th>").arg(width, value);
		}
		mContent += QLatin1String("</tr>");
	}
}


void PdfCreator::addTableRow(const QStringList& pValues)
{
	mContent += mColoredRow ? QLatin1String("<tr style='background-color:#f6f6f6'>") : QLatin1String("<tr>");
	for (int i = 0; i < mColumnCount; ++i)
	{
		const auto& value = i < pValues.size() && pValues[i] != "" ? pValues[i] : QLatin1String("&nbsp;");
		mContent += QStringLiteral("<td>%2</td>").arg(value);
	}
	mContent += QLatin1String("</tr>");
}


void PdfCreator::toggleRowColor()
{
	mColoredRow = !mColoredRow;
}


void PdfCreator::save()
{
	closeTable();

	QPrinter printer;
	printer.setOutputFileName(mFilename);
	QRect printer_rect(printer.pageRect());

	QString company_header;
	company_header += QStringLiteral("<br><br>"
									 "<table cellspacing='5' cellpadding='5' width='100%'>"
									 "<tr>"
									 "<td valign='middle'>"
									 "<h1>"
									 "%1 - %2"
									 "</h1>"
									 "<h3>"
									 "%3"
									 "<th rowspan='2' width='100'>"
									 "<img src='pdflogo' width='100' height='100'>"
									 "</th>"
									 "</h3>"
									 "</td>"
									 "</tr>"
									 "<tr>"
									 "<td valign='middle'>"
									 "<br>"
									 "<h3>"
									 "%4"
									 "</h3>"
									 "</td>"
									 "</tr>"
									 "</table>").arg(
			tr("History"),
			QApplication::applicationName(),
			tr("AusweisApp2 is a product of Governikus GmbH & Co. KG - on behalf of the Bundesministerium des Innern (Federal Ministry of the Interior)."),
			mHeadline);

	QString company_footer;
	company_footer += QLatin1String("<h3>")
			% tr("For further information, please see <a href='https://www.ausweisapp.bund.de/'>https://www.ausweisapp.bund.de/</a>")
			% QLatin1String("</h3>");

	//Setting up the header and calculating the header size
	QTextDocument document_header;

	QSvgRenderer renderer(QStringLiteral(":/images/npa.svg"));
	QImage image(768, 768, QImage::Format_ARGB32);
	image.fill(0x00FFFFFF);
	QPainter imagePainter(&image);
	renderer.render(&imagePainter);
	document_header.addResource(QTextDocument::ImageResource, QUrl(QStringLiteral("pdflogo")), image);

	document_header.setPageSize(printer_rect.size());
	document_header.setHtml(company_header);
	QSizeF header_size = document_header.size();

	//Setting up the footer and calculating the footer size
	QTextDocument document_footer;
	document_footer.setPageSize(printer_rect.size());
	document_footer.setHtml(company_footer);
	QSizeF footer_size = document_footer.size();

	//Calculating the main document size for one page
	QSizeF center_size(printer_rect.width(),
			(printer.pageRect().height() - header_size.toSize().height() - footer_size.toSize().height()));

	//Insert HTML in main document
	QTextDocument main_doc;
	main_doc.setHtml(mContent);
	main_doc.setPageSize(center_size);

	//Setting up the rectangles for each section.
	QRect headerRect = QRect(QPoint(0, 0), document_header.size().toSize());
	QRect footerRect = QRect(QPoint(0, 0), document_footer.size().toSize());
	QRect contentRect = QRect(QPoint(0, 0), main_doc.size().toSize()); // Main content rectangle.
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
		main_doc.drawContents(&painter, currentRect);
		//Resetting the painter matrix co ordinate system.
		painter.resetMatrix();
		//Drawing the header on the top of the page
		document_header.drawContents(&painter, headerRect);
		//Applying positive translation of painter co-ordinate system to draw the footer
		painter.translate(0, headerRect.height());
		painter.translate(0, center_size.height());
		document_footer.drawContents(&painter, footerRect);

		//Translating the current rectangle to the area to be printed for the next page
		currentRect.translate(0, currentRect.height());
		//Inserting a new page if there is still area left to be printed
		if (currentRect.intersects(contentRect))
		{
			printer.newPage();
		}
	}
}
