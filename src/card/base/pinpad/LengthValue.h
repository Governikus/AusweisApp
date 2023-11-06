/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Functions to read and write PCSC messages with a length-value encoding.
 */

#pragma once

#include <QByteArray>
#include <QtEndian>

#include <algorithm>
#include <climits>


namespace governikus
{

class LengthValue
{
	private:
		LengthValue() = delete;
		~LengthValue() = delete;

	public:
		template<typename T> static QByteArray readByteArray(const QByteArray& pInput, int& pOffset)
		{
			Q_ASSERT(sizeof(T) < INT_MAX);

			const auto typeLength = static_cast<int>(sizeof(T));
			if (pInput.size() < pOffset + typeLength)
			{
				return QByteArray();
			}

			const T length = qFromLittleEndian<T>(pInput.data() + pOffset);
			pOffset += typeLength;
			const QByteArray result = pInput.mid(pOffset, length);
			pOffset += length;
			return result;
		}


		template<typename T> static void writeByteArray(const QByteArray& pValue, QByteArray& pOutput)
		{
			Q_ASSERT(sizeof(T) < INT_MAX);

			const int maxSize = std::numeric_limits<T>::max();
			const int size = std::min(maxSize, static_cast<int>(pValue.size()));

			const auto it = pOutput.size();
			pOutput.resize(it + static_cast<int>(sizeof(T)));
			qToLittleEndian(static_cast<T>(size), pOutput.data() + it);
			pOutput += pValue.mid(0, size);
		}


};

} // namespace governikus
