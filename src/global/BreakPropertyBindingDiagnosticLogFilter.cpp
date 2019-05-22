/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "BreakPropertyBindingDiagnosticLogFilter.h"

using namespace governikus;


QLoggingCategory::CategoryFilter BreakPropertyBindingDiagnosticLogFilter::cOldFilterFunctionPointer = nullptr;

BreakPropertyBindingDiagnosticLogFilter::BreakPropertyBindingDiagnosticLogFilter(QObject* pParent)
	: QObject(pParent)
{
	cOldFilterFunctionPointer = QLoggingCategory::installFilter(filterFunction);
	Q_ASSERT(cOldFilterFunctionPointer != filterFunction && "Created a filter loop");
}


void BreakPropertyBindingDiagnosticLogFilter::filterFunction(QLoggingCategory* pCategory)
{
	const QLatin1String breakPropertyBindingDiagnostic("qt.qml.binding.removal");
	if (QLatin1String(pCategory->categoryName()) == breakPropertyBindingDiagnostic)
	{
		pCategory->setEnabled(QtInfoMsg, true);
		return;
	}

	if (cOldFilterFunctionPointer == nullptr)
	{
		return;
	}
	cOldFilterFunctionPointer(pCategory);
}
