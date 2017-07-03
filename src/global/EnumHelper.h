/*!
 * EnumHelper.h
 *
 * \brief Helper class to provide a QMetaObject handler for enumerations.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDebug>
#include <QMetaEnum>

namespace governikus
{

template<typename T> struct Enum {};

#define defineTypedEnumType(enumName, enumType, ...)\
	class Enum##enumName\
	{\
		Q_GADGET\
		private:\
			Enum##enumName();\
			Q_DISABLE_COPY(Enum##enumName)\
\
		public:\
			enum class enumName : enumType\
			{\
				__VA_ARGS__\
			};\
\
			Q_ENUM(enumName)\
\
			static inline QMetaEnum getMetaEnum()\
			{\
				return QMetaEnum::fromType<enumName>();\
			}\
			static QString getName()\
			{\
				return QString::fromLatin1(getMetaEnum().name());\
			}\
			static QString getName(enumName pType)\
			{\
				const char* name = getMetaEnum().valueToKey(static_cast<int>(pType));\
				if (name == nullptr)\
				{\
					return QStringLiteral("UNKNOWN(0x%1)").arg(static_cast<int>(pType), 0, 16);\
				}\
				else\
				{\
					return QString::fromLatin1(name);\
				}\
			}\
\
			static int getCount()\
			{\
				return getMetaEnum().keyCount();\
			}\
\
			static const QVector<enumName>& getList()\
			{\
				static QVector<enumName> list;\
				if (list.isEmpty())\
				{\
					const QMetaEnum metaEnum = getMetaEnum();\
					list.reserve(metaEnum.keyCount());\
					for (int i = 0; i < metaEnum.keyCount(); ++i)\
					{\
						list.push_back(static_cast<enumName>(metaEnum.value(i)));\
					}\
				}\
				return list;\
			}\
			static enumName fromString(const char* pValue, enumName pDefault)\
			{\
				bool ok = false;\
				int key = getMetaEnum().keyToValue(pValue, &ok);\
				if (ok)\
				{\
					return static_cast<enumName>(key);\
				}\
				return pDefault;\
			}\
			static enumName fromString(const QString &pValue, enumName pDefault)\
			{\
				return fromString(pValue.toUtf8().constData(), pDefault);\
			}\
			static bool isValue(int pValue)\
			{\
				return getMetaEnum().valueToKey(pValue) != nullptr;\
			}\
			static bool isValue(uchar pValue)\
			{\
				return isValue(static_cast<int>(pValue));\
			}\
			static bool isValue(char pValue)\
			{\
				return isValue(static_cast<uchar>(pValue));\
			}\
			static enumType getValue(enumName pType)\
			{\
				return static_cast<enumType>(pType);\
			}\
	};\
\
	typedef Enum##enumName::enumName enumName;\
\
	template<> struct Enum<enumName> {\
		static QString getName()\
		{\
			return Enum##enumName::getName();\
		}\
\
		static QString getName(enumName pType)\
		{\
			return Enum##enumName::getName(pType);\
		}\
\
		static int getCount()\
		{\
			return Enum##enumName::getCount();\
		}\
		static const QVector<enumName>& getList()\
		{\
			return Enum##enumName::getList();\
		}\
		static enumName fromString(const char* pValue, enumName pDefault)\
		{\
			return Enum##enumName::fromString(pValue, pDefault);\
		}\
		static enumName fromString(const QString &pValue, enumName pDefault)\
		{\
			return Enum##enumName::fromString(pValue, pDefault);\
		}\
		static bool isValue(int pValue)\
		{\
			return Enum##enumName::isValue(pValue);\
		}\
		static bool isValue(uchar pValue)\
		{\
			return Enum##enumName::isValue(pValue);\
		}\
		static bool isValue(char pValue)\
		{\
			return Enum##enumName::isValue(pValue);\
		}\
		static enumType getValue(enumName pType)\
		{\
			return Enum##enumName::getValue(pType);\
		}\
	};\
\
	inline QDebug operator<<(QDebug pDbg, enumName pType)\
	{\
		pDbg << Enum##enumName::getName(pType);\
		return pDbg;\
	}\
\
	inline QString& operator+=(QString& pStr, enumName pType)\
	{\
		pStr += Enum##enumName::getName(pType);\
		return pStr;\
	}\
\
	inline QString operator+(const QString& pStr, enumName pType)\
	{\
		return pStr + Enum##enumName::getName(pType);\
	}\
\
	inline QString operator+(enumName pType, const QString& pStr)\
	{\
		return Enum##enumName::getName(pType) + pStr;\
	}\
\
	inline bool operator==(enumType pType, enumName pName)\
	{\
		return static_cast<enumType>(pName) == pType;\
	}\
	inline bool operator!=(enumType pType, enumName pName)\
	{\
		return !(pType == pName);\
	}


#define defineEnumType(enumName, ...) defineTypedEnumType(enumName, int, __VA_ARGS__)

template<typename T> inline QString getEnumName(T pType)
{
	return Enum<T>::getName(pType);
}


template<typename T> inline QByteArray getEnumByteValue(T pType)
{
	QByteArray data;
	data.append(Enum<T>::getValue(pType));
	return data;
}


} /* namespace governikus */
