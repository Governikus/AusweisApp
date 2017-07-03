/*!
 * \brief Tool to create PDF-Documents for history or selfauthentication result.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QCoreApplication>
#include <QObject>
#include <QString>

namespace governikus
{

class PdfExport
{
	Q_DECLARE_TR_FUNCTIONS(governikus::PdfExport)

	public:
		static bool exportHistory(const QString& pFileName);
};


class PdfCreator
{
	Q_DECLARE_TR_FUNCTIONS(governikus::PdfCreator)

	private:
		QString mFilename;
		bool mColoredRow;
		int mColumnCount;
		QString mHeadline;
		QString mContent;

		void closeTable();

	public:
		PdfCreator(const QString& pFilename);
		virtual ~PdfCreator();

		QUrl getFileUrl();

		void setHeadline(const QString& pHeadline);
		void initTable(int pColumnCount, const QList<int>& pWidth, const QStringList& pValues);
		void addTableRow(const QStringList& pValues);
		void toggleRowColor();
		void save();

};


} /* namespace governikus */
