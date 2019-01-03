/*
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLoggingCategory>
#include <QObject>

namespace governikus
{

class BreakPropertyBindingDiagnosticLogFilter
	: public QObject
{
	Q_OBJECT

	private:
		static QLoggingCategory::CategoryFilter cOldFilterFunctionPointer;

		static void filterFunction(QLoggingCategory* pCategory);

	public:
		explicit BreakPropertyBindingDiagnosticLogFilter(QObject* pParent);
};

} // namespace governikus
