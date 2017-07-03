/*!
 * MockQtthpConnection.h
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "qhttpserver/qhttpconnection.h"


namespace governikus
{

class MockQHttpConnection
	: public QHttpConnection
{
	Q_OBJECT

	public:
		QByteArray mContent;

		MockQHttpConnection();
		virtual ~MockQHttpConnection();

		void write(const QByteArray& data) override;


};

} /* namespace governikus */
