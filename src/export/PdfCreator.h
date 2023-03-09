/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tool to create PDF-Documents.
 */

#pragma once

#include <QCoreApplication>
#include <QPainter>
#include <QPdfWriter>
#include <QString>
#include <QTextDocument>

namespace governikus
{

class PdfCreator
{
	Q_DECLARE_TR_FUNCTIONS(governikus::PdfCreator)

	private:
		QPdfWriter mPdfWriter;
		QTextDocument mHeader;
		QTextDocument mContent;
		QTextDocument mFooter;

		void drawContents(const QTextDocument& pTextDocument, QPainter& pPainter, const QRectF& pClipRect = QRectF());
		void createHeader(const QString& pTitle, const QString& pHeadline);
		void createContent(const QString& pContent);
		void createFooter();

	public:
		PdfCreator(const QString& pFilename, const QString& pTitle, const QString& pHeadline, const QString& pContent);
		bool save();
};


} // namespace governikus
