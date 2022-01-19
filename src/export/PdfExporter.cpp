/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "PdfExporter.h"

#include "asn1/AccessRoleAndRight.h"
#include "AppSettings.h"
#include "LanguageLoader.h"
#include "PdfCreator.h"

#include <QDesktopServices>

using namespace governikus;

namespace
{
QString getValueOrWhitespace(const QStringList& pValues, int i)
{
	if (i < pValues.size() && !pValues.at(i).isEmpty())
	{
		return pValues.at(i);
	}
	return QStringLiteral("&nbsp;");
}


} // namespace


PdfExporter::PdfExporter(const QString& pFilename, bool pOpenFile, bool pFixFilename)
	: mFilename(pFilename)
	, mOpenFile(pOpenFile)
	, mColoredRow(false)
	, mColumnCount(0)
	, mContent()
{
	if (pFixFilename && !mFilename.isEmpty()
			&& !mFilename.endsWith(QLatin1String(".pdf"), Qt::CaseInsensitive))
	{
		mFilename += QStringLiteral(".pdf");
	}
}


QString PdfExporter::getContent() const
{
	return mContent.join(QString());
}


void PdfExporter::checkOpenFile(bool pSuccess)
{
	if (mOpenFile && pSuccess)
	{
		QDesktopServices::openUrl(QUrl(QStringLiteral("file:///") + mFilename));
	}
}


void PdfExporter::initTable(int pColumnCount, const QList<int>& pWidth, const QStringList& pValues)
{
	Q_ASSERT(mColumnCount == 0);

	mColumnCount = pColumnCount;
	mContent << QStringLiteral("<table width='100%' cellspacing='0' cellpadding='6'><tr>");
	for (int i = 0; i < mColumnCount; ++i)
	{
		const auto& width = i < pWidth.size() ? QStringLiteral(" width='%1'").arg(pWidth.at(i)) : QString();
		mContent << QStringLiteral("<th align='left'%1>%2</th>").arg(width, getValueOrWhitespace(pValues, i));
	}
	mContent << QStringLiteral("</tr>");
}


void PdfExporter::addTableRow(const QStringList& pValues)
{
	mContent << (mColoredRow ? QStringLiteral("<tr style='background-color:#f6f6f6'>") : QStringLiteral("<tr>"));
	for (int i = 0; i < mColumnCount; ++i)
	{
		mContent << QStringLiteral("<td>%2</td>").arg(getValueOrWhitespace(pValues, i));
	}
	mContent << QStringLiteral("</tr>");
}


void PdfExporter::closeTable()
{
	mContent << QStringLiteral("</table>");
	mColumnCount = 0;
	mColoredRow = false;
}


void PdfExporter::toggleRowColor()
{
	mColoredRow = !mColoredRow;
}


bool PdfExporter::exportHistory()
{
	if (mFilename.isEmpty())
	{
		return false;
	}
	mContent.clear();

	const auto& locale = LanguageLoader::getInstance().getUsedLocale();

	initTable(3, {180, 80},
			//: LABEL ALL_PLATFORMS
			{tr("Date"),
			 //: LABEL ALL_PLATFORMS
			 tr("Details")});
	//: LABEL ALL_PLATFORMS
	const auto& dateTimeFormat = tr("dd.MM.yyyy hh:mm AP");
	const auto& infos = Env::getSingleton<AppSettings>()->getHistorySettings().getHistoryInfos();
	for (const auto& entry : infos)
	{
		toggleRowColor();
		const QString& dateTimeEntry = locale.toString(entry.getDateTime(), dateTimeFormat);
		//: LABEL ALL_PLATFORMS
		addTableRow({dateTimeEntry, tr("Provider:"), entry.getSubjectName()});
		//: LABEL ALL_PLATFORMS
		addTableRow({QString(), tr("Purpose:"), entry.getPurpose()});

		const auto& readData = AccessRoleAndRightsUtil::joinFromTechnicalName(entry.getRequestedData(), AccessRoleAndRightsUtil::READ);
		if (!readData.isEmpty())
		{
			//: LABEL ALL_PLATFORMS
			addTableRow({QString(), tr("Read access:"), readData});
		}

		const auto& writtenData = AccessRoleAndRightsUtil::joinFromTechnicalName(entry.getRequestedData(), AccessRoleAndRightsUtil::WRITE);
		if (!writtenData.isEmpty())
		{
			//: LABEL ALL_PLATFORMS
			addTableRow({QString(), tr("Write access (update):"), writtenData});
		}
	}
	closeTable();

	const auto& now = QDateTime::currentDateTime();
	//: LABEL ALL_PLATFORMS
	QString date = locale.toString(now, tr("dd.MM.yyyy"));
	//: LABEL ALL_PLATFORMS
	QString time = locale.toString(now, tr("hh:mm AP"));
	//: LABEL ALL_PLATFORMS
	const auto& headline = tr("At %1 %2 the following data were saved:").arg(date, time);

	//: LABEL ALL_PLATFORMS
	PdfCreator pdf(mFilename, tr("History"), headline, getContent());
	const bool success = pdf.save();
	checkOpenFile(success);
	return success;
}


bool PdfExporter::exportSelfInfo(const QDateTime& pDate, const QVector<QPair<QString, QString> >& pInfoData)
{
	if (mFilename.isEmpty())
	{
		return false;
	}
	mContent.clear();

	initTable(2, {180},
			//: LABEL ALL_PLATFORMS
			{tr("Entry"),
			 //: LABEL ALL_PLATFORMS
			 tr("Content")});
	for (const auto& entry : pInfoData)
	{
		if (!entry.first.isEmpty())
		{
			toggleRowColor();
		}
		addTableRow({entry.first, entry.second});
	}
	closeTable();

	const auto& locale = LanguageLoader::getInstance().getUsedLocale();
	//: LABEL ALL_PLATFORMS
	QString date = locale.toString(pDate, tr("dd.MM.yyyy"));
	//: LABEL ALL_PLATFORMS
	QString time = locale.toString(pDate, tr("hh:mm AP"));
	//: LABEL ALL_PLATFORMS
	const auto& headline = tr("At %1 %2 the following data has been read out of your ID card:").arg(date, time);

	//: LABEL ALL_PLATFORMS
	PdfCreator pdf(mFilename, tr("Information"), headline, getContent());
	const bool success = pdf.save();
	checkOpenFile(success);
	return success;
}
