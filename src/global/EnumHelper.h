/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDebug>
#include <QMetaEnum>
#include <type_traits>


#define defineEnumOperators(enumName)\
		inline QDebug operator<<(QDebug pDbg, enumName pType)\
		{\
			QDebugStateSaver saver(pDbg);\
			return pDbg.noquote() << Enum<enumName>::getName(pType);\
		}\
\
		inline QDebug operator<<(QDebug pDbg, const QList<enumName>& pList)\
		{\
			QDebugStateSaver saver(pDbg);\
			QByteArrayList list;\
			for (const auto& entry : pList)\
			{\
				list << Enum<enumName>::getName(entry).data();\
			}\
			return pDbg.noquote().nospace() << '(' << list.join(QByteArrayView(", ")) << ')';\
		}\
\
		inline QString& operator+=(QString& pStr, enumName pType)\
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
\
		inline size_t qHash(enumName pKey, size_t pSeed)\
		{\
			return ::qHash(static_cast<std::underlying_type_t<enumName>>(pKey), pSeed);\
		}


#define defineTypedEnumTypeProperty(enumName, enumType, enumProperty, ...)\
		namespace Enum##enumName\
		{\
		Q_NAMESPACE\
				enumProperty\
\
		enum class enumName : enumType\
		{\
			__VA_ARGS__\
		};\
\
		Q_ENUM_NS(enumName)\
\
		defineEnumOperators(enumName)\
		}\
\
		using namespace Enum##enumName;


#define defineTypedEnumType(enumName, enumType, ...) defineTypedEnumTypeProperty(enumName, enumType, , __VA_ARGS__)
#define defineEnumType(enumName, ...) defineTypedEnumType(enumName, int, __VA_ARGS__)

/* *INDENT-OFF* */
#define ENUM_HELPER_OP (
#define ENUM_HELPER_CP )
#define ENUM_HELPER_CO ,
/* *INDENT-ON* */
#define defineEnumTypeQmlExposed(enumName, ...) defineTypedEnumTypeProperty(enumName, int, Q_CLASSINFO ENUM_HELPER_OP "QML.Element" ENUM_HELPER_CO #enumName ENUM_HELPER_CP, __VA_ARGS__)


namespace governikus
{

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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
			const auto value = static_cast<quint64>(pType);
#else
			const auto value = static_cast<int>(pType);
#endif
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


		[[nodiscard]] static QList<EnumTypeT> getList()
		{
			QList<EnumTypeT> list;

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


#if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
		[[nodiscard]] static bool isValue(quint64 pValue)
#else
		[[nodiscard]] static bool isValue(int pValue)
#endif
		{
			return getQtEnumMetaEnum().valueToKey(pValue) != nullptr;
		}


#if (QT_VERSION < QT_VERSION_CHECK(6, 9, 0))
		[[nodiscard]] static bool isValue(ushort pValue)
		{
			return isValue(static_cast<int>(pValue));
		}


#endif

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
