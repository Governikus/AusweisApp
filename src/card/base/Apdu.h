/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
		[[nodiscard]] const QByteArray& getBuffer() const;
		[[nodiscard]] int length() const;
		[[nodiscard]] bool isEmpty() const;
};


} // namespace governikus
