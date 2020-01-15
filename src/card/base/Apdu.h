/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>

namespace governikus
{

class Apdu
{
	protected:
		QByteArray mBuffer;

		explicit Apdu(const QByteArray& pBuffer);
		~Apdu() = default;

	public:
		const QByteArray& getBuffer() const;
		int length() const;
		bool isEmpty() const;
};


} // namespace governikus
