/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "WebSocketServer.h"

namespace governikus
{

class LocalWebSocketServer
	: public WebSocketServer
{
	Q_OBJECT

	public:
		~LocalWebSocketServer() override;

		virtual void setPsk(const QByteArray& pPsk) = 0;

};

} // namespace governikus
