/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
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
	: public QObject
{
	Q_OBJECT
	friend class ::test_RedirectRequest;

	private:
		QTcpSocket mSocket;
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
