/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the authentication process.
 */

#pragma once

#include "WorkflowController.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"

namespace governikus
{

class AuthController
	: public WorkflowController
{
	Q_OBJECT

	public:
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(const QUrl& pUrl,
				const QVariant& pData = QVariant(),
				const AuthContext::BrowserHandler& pBrowserHandler = AuthContext::BrowserHandler());

		explicit AuthController(QSharedPointer<AuthContext> pContext);
		~AuthController() override = default;
};

} // namespace governikus
