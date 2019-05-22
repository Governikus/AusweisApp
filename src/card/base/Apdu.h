/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>

namespace governikus
{

class Apdu
{
	protected:
		QByteArray mBuffer;

		Apdu(const QByteArray& pBuffer);
		~Apdu() = default;

	public:
		const QByteArray& getBuffer() const;
		int length() const;
};


} // namespace governikus
