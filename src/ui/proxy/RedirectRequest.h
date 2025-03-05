/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpRequest.h"
#include "PortWrapper.h"

#include <QSharedPointer>
#include <QTcpSocket>

class test_RedirectRequest;

namespace governikus
{
class RedirectRequest
	: private QTcpSocket
{
	Q_OBJECT
	friend class ::test_RedirectRequest;

	private:
		QSharedPointer<HttpRequest> mRequest;
		PortWrapper mPortWrapper;
		bool mAnswerReceived;

		void sendHttpRedirect();
		void redirect();
		void answerReceived();
		[[nodiscard]] bool isAnswerReceived() const;

	public:
		explicit RedirectRequest(const QSharedPointer<HttpRequest>& pRequest, QObject* pParent = nullptr);
		~RedirectRequest() override;
};

} // namespace governikus
