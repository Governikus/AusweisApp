/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MessageDispatcher.h"

#include "TestWorkflowContext.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

void setValidState(MessageDispatcher& pDispatcher,
		bool pSelectReader,
		bool pBasicReader,
		const PacePasswordId pPasswordID,
		const QLatin1String& pState = QLatin1String("StateEnterPacePassword"),
		const QSharedPointer<WorkflowContext> pContext = QSharedPointer<TestWorkflowContext>::create());

QByteArray addReaderData(const char* pData, bool pKeyPad = false);


} // namespace governikus
