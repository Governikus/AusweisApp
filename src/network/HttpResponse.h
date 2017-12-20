/*!
 * \brief Class to create http response.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "HttpStatusCode.h"

#include <QByteArray>
#include <QMap>

namespace governikus
{

class HttpResponse
{
	private:
		HttpStatusCode mStatus;
		QMap<QByteArray, QByteArray> mHeader;
		QByteArray mBody;

	public:
		HttpResponse(HttpStatusCode pStatus = HttpStatusCode::UNDEFINED, const QByteArray& pBody = QByteArray(), const QByteArray& pContentType = QByteArray());
		virtual ~HttpResponse();
		bool isValid() const;

		QByteArray getHeader(const QByteArray& pKey) const;
		const QMap<QByteArray, QByteArray>& getHeaders() const;
		void setHeader(const QByteArray& pKey, const QByteArray& pValue);

		HttpStatusCode getStatus() const;
		void setStatus(HttpStatusCode pStatus);

		const QByteArray& getBody() const;
		void setBody(const QByteArray& pBody, const QByteArray& pContentType = QByteArray());

		QByteArray getMessage() const;
};

} /* namespace governikus */
