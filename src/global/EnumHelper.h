/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper class to provide a QMetaObject handler for enumerations.
 */

#pragma once

#include "Initializer.h"

#include <QDebug>
#include <QMetaEnum>
#include <type_traits>

namespace governikus
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	#define defineQHash(enumName)\
	inline size_t qHash(enumName pKey, size_t pSeed)\
	{\
		return ::qHash(static_cast<std::underlying_type_t<enumName>>(pKey), pSeed);\
	}
#else
	#define defineQHash(enumName)\
	inline uint qHash(enumName pKey, uint pSeed)\
	{\
		return ::qHash(static_cast<std::underlying_type_t<enumName>>(pKey), pSeed);\
	}
#endif

#define defineEnumOperators(enumName)\
	inline QDebug operator<<(QDebug pDbg, enumName pType)\
	{\
		QDebugStateSaver saver(pDbg);\
		return pDbg.noquote() << Enum<enumName>::getName(pType);\
	}\
\
	inline QString& operator+=(QString & pStr, enumName pType)\
	{\
		pStr += Enum<enumName>::getName(pType);\
		return pStr;\
	}\
\
	inline QString operator+(const QString& pStr, enumName pType)\
	{\
		return pStr + Enum<enumName>::getName(pType);\
	}\
\
	inline QString operator+(enumName pType, const QString& pStr)\
	{\
		return Enum<enumName>::getName(pType) + pStr;\
	}\
\
	inline bool operator==(std::underlying_type_t<enumName> pType, enumName pName)\
	{\
		return static_cast<std::underlying_type_t<enumName>>(pName) == pType;\
	}\
	inline bool operator!=(std::underlying_type_t<enumName> pType, enumName pName)\
	{\
		return !(pType == pName);\
	}\
	defineQHash(enumName)


#define defineTypedEnumType(enumName, enumType, ...)\
	class Enum##enumName\
	{\
		Q_GADGET\
\
		Q_DISABLE_COPY(Enum##enumName)\
\
		private:\
			Enum##enumName();\
\
		public:\
			enum class enumName : enumType\
			{\
				__VA_ARGS__\
			};\
\
			Q_ENUM(enumName)\
	};\
\
	using enumName = Enum##enumName::enumName;\
	namespace governikusEnum##enumName\
	{\
	INIT_FUNCTION([]\
		{\
			qRegisterMetaType<enumType>(#enumName);\
		})\
	}\
\
	defineEnumOperators(enumName)


#define defineEnumType(enumName, ...) defineTypedEnumType(enumName, int, __VA_ARGS__)


template<typename EnumTypeT> class Enum
{
	using EnumBaseTypeT = std::underlying_type_t<EnumTypeT>;

	Q_DISABLE_COPY(Enum)

	private:
		Enum() = delete;
		~Enum() = delete;

	public:
		[[nodiscard]] static inline QMetaEnum getQtEnumMetaEnum()
		{
			return QMetaEnum::fromType<EnumTypeT>();
		}


		[[nodiscard]] static QLatin1String getName()
		{
			return QLatin1String(getQtEnumMetaEnum().name());
		}


		[[nodiscard]] static QLatin1String getName(EnumTypeT pType)
		{
			const auto value = static_cast<int>(pType);
			const char* const name = getQtEnumMetaEnum().valueToKey(value);
			if (Q_UNLIKELY(name == nullptr))
			{
				qCritical().noquote().nospace() << "CRITICAL CONVERSION MISMATCH: UNKNOWN 0x" << QString::number(value, 16);
				return QLatin1String();
			}

			return QLatin1String(name);
		}


		[[nodiscard]] static int getCount()
		{
			return getQtEnumMetaEnum().keyCount();
		}


		[[nodiscard]] static QVector<EnumTypeT> getList()
		{
			QVector<EnumTypeT> list;

			const QMetaEnum metaEnum = getQtEnumMetaEnum();
			list.reserve(metaEnum.keyCount());
			for (int i = 0; i < metaEnum.keyCount(); ++i)
			{
				list << static_cast<EnumTypeT>(metaEnum.value(i));
			}

			return list;
		}


		[[nodiscard]] static EnumTypeT fromString(const char* const pValue, EnumTypeT pDefault)
		{
			bool ok = false;
			int key = getQtEnumMetaEnum().keyToValue(pValue, &ok);
			if (ok)
			{
				return static_cast<EnumTypeT>(key);
			}
			return pDefault;
		}


		[[nodiscard]] static EnumTypeT fromString(const QString& pValue, EnumTypeT pDefaultType)
		{
			return fromString(pValue.toUtf8().constData(), pDefaultType);
		}


		[[nodiscard]] static bool isValue(int pValue)
		{
			return getQtEnumMetaEnum().valueToKey(pValue) != nullptr;
		}


		[[nodiscard]] static bool isValue(ushort pValue)
		{
			return isValue(static_cast<int>(pValue));
		}


		[[nodiscard]] static EnumBaseTypeT getValue(EnumTypeT pType)
		{
			return static_cast<EnumBaseTypeT>(pType);
		}


};


template<typename T> inline QLatin1String getEnumName(T pType)
{
	return Enum<T>::getName(pType);
}


} // namespace governikus
