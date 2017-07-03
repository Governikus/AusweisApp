/*
 * Copyright 2011-2014 Nikhil Marathe <nsm.nikhil@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "qhttpserver.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QVariant>
#include <QDebug>

#include "qhttpconnection.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"

QHash<int, QString> STATUS_CODES;

QHttpServer::QHttpServer(QObject* pParent)
	: QObject(pParent)
	, m_tcpServer(nullptr)
{
	STATUS_CODES.insert(100, QStringLiteral("Continue"));
	STATUS_CODES.insert(101, QStringLiteral("Switching Protocols"));
	STATUS_CODES.insert(102, QStringLiteral("Processing"));
	// RFC 2518) obsoleted by RFC 4918
	STATUS_CODES.insert(200, QStringLiteral("OK"));
	STATUS_CODES.insert(201, QStringLiteral("Created"));
	STATUS_CODES.insert(202, QStringLiteral("Accepted"));
	STATUS_CODES.insert(203, QStringLiteral("Non-Authoritative Information"));
	STATUS_CODES.insert(204, QStringLiteral("No Content"));
	STATUS_CODES.insert(205, QStringLiteral("Reset Content"));
	STATUS_CODES.insert(206, QStringLiteral("Partial Content"));
	STATUS_CODES.insert(207, QStringLiteral("Multi-Status"));
	// RFC 4918
	STATUS_CODES.insert(300, QStringLiteral("Multiple Choices"));
	STATUS_CODES.insert(301, QStringLiteral("Moved Permanently"));
	STATUS_CODES.insert(302, QStringLiteral("Moved Temporarily"));
	STATUS_CODES.insert(303, QStringLiteral("See Other"));
	STATUS_CODES.insert(304, QStringLiteral("Not Modified"));
	STATUS_CODES.insert(305, QStringLiteral("Use Proxy"));
	STATUS_CODES.insert(307, QStringLiteral("Temporary Redirect"));
	STATUS_CODES.insert(400, QStringLiteral("Bad Request"));
	STATUS_CODES.insert(401, QStringLiteral("Unauthorized"));
	STATUS_CODES.insert(402, QStringLiteral("Payment Required"));
	STATUS_CODES.insert(403, QStringLiteral("Forbidden"));
	STATUS_CODES.insert(404, QStringLiteral("Not Found"));
	STATUS_CODES.insert(405, QStringLiteral("Method Not Allowed"));
	STATUS_CODES.insert(406, QStringLiteral("Not Acceptable"));
	STATUS_CODES.insert(407, QStringLiteral("Proxy Authentication Required"));
	STATUS_CODES.insert(408, QStringLiteral("Request Time-out"));
	STATUS_CODES.insert(409, QStringLiteral("Conflict"));
	STATUS_CODES.insert(410, QStringLiteral("Gone"));
	STATUS_CODES.insert(411, QStringLiteral("Length Required"));
	STATUS_CODES.insert(412, QStringLiteral("Precondition Failed"));
	STATUS_CODES.insert(413, QStringLiteral("Request Entity Too Large"));
	STATUS_CODES.insert(414, QStringLiteral("Request-URI Too Large"));
	STATUS_CODES.insert(415, QStringLiteral("Unsupported Media Type"));
	STATUS_CODES.insert(416, QStringLiteral("Requested Range Not Satisfiable"));
	STATUS_CODES.insert(417, QStringLiteral("Expectation Failed"));
	STATUS_CODES.insert(418, QStringLiteral("I\"m a teapot"));
	// RFC 2324
	STATUS_CODES.insert(422, QStringLiteral("Unprocessable Entity"));
	// RFC 4918
	STATUS_CODES.insert(423, QStringLiteral("Locked"));
	// RFC 4918
	STATUS_CODES.insert(424, QStringLiteral("Failed Dependency"));
	// RFC 4918
	STATUS_CODES.insert(425, QStringLiteral("Unordered Collection"));
	// RFC 4918
	STATUS_CODES.insert(426, QStringLiteral("Upgrade Required"));
	// RFC 2817
	STATUS_CODES.insert(500, QStringLiteral("Internal Server Error"));
	STATUS_CODES.insert(501, QStringLiteral("Not Implemented"));
	STATUS_CODES.insert(502, QStringLiteral("Bad Gateway"));
	STATUS_CODES.insert(503, QStringLiteral("Service Unavailable"));
	STATUS_CODES.insert(504, QStringLiteral("Gateway Time-out"));
	STATUS_CODES.insert(505, QStringLiteral("HTTP Version not supported"));
	STATUS_CODES.insert(506, QStringLiteral("Variant Also Negotiates"));
	// RFC 2295
	STATUS_CODES.insert(507, QStringLiteral("Insufficient Storage"));
	// RFC 4918
	STATUS_CODES.insert(509, QStringLiteral("Bandwidth Limit Exceeded"));
	STATUS_CODES.insert(510, QStringLiteral("Not Extended"));
	// RFC 2774
}


QHttpServer::~QHttpServer()
{
}


void QHttpServer::newConnection()
{
	Q_ASSERT(m_tcpServer);

	while (m_tcpServer->hasPendingConnections())
	{
		QHttpConnection* connection = new QHttpConnection(m_tcpServer->nextPendingConnection());
		connect(connection, &QHttpConnection::newRequest, this, &QHttpServer::newRequest);
	}
}


bool QHttpServer::listen(const QHostAddress& address, quint16 port)
{
	Q_ASSERT(!m_tcpServer);
	m_tcpServer = new QTcpServer(this);

	bool couldBindToPort = m_tcpServer->listen(address, port);
	if (couldBindToPort)
	{
		connect(m_tcpServer, &QTcpServer::newConnection, this, &QHttpServer::newConnection);
	}
	else
	{
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}
	return couldBindToPort;
}


bool QHttpServer::listen(quint16 port)
{
	return listen(QHostAddress::Any, port);
}


quint16 QHttpServer::serverPort()
{
	Q_ASSERT(m_tcpServer);

	if (m_tcpServer)
	{
		return m_tcpServer->serverPort();
	}

	return 0;
}


void QHttpServer::close()
{
	if (m_tcpServer)
	{
		m_tcpServer->close();
	}
}
