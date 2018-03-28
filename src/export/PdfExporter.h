/*!
 * \brief Tool to export data of history or selfauthentication result.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

namespace governikus
{
class PdfExporter
{
	Q_DECLARE_TR_FUNCTIONS(governikus::PdfExporter)

	private:
		QString mFilename;
		bool mOpenFile;
		bool mColoredRow;
		int mColumnCount;
		QStringList mContent;

		QString getContent() const;

		void checkOpenFile(bool pSuccess);
		void initTable(int pColumnCount, const QList<int>& pWidth, const QStringList& pValues);
		void closeTable();
		void addTableRow(const QStringList& pValues);
		void toggleRowColor();

	public:
		PdfExporter(const QString& pFilename, bool pOpenFile = true, bool pFixFilename = true);
		bool exportHistory();
		bool exportSelfInfo(const QDateTime& pDate, const QVector<QPair<QString, QString> >& pInfoData);
};

} /* namespace governikus */
