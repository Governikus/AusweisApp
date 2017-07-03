/*!
 * MetaTypeHelper.h
 *
 * \brief Helper macros REGISTER_META_TYPE() and REGISTER_POINTER_META_TYPE() to register meta types.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QMetaEnum>
#include <QObject>

template<typename Class>
struct RegisterMetaType
{
	RegisterMetaType(const char* name, const char* qualifiedName)
	{
		// Note: In theory registering the qualified name should suffice, but
		// apparently sometimes Qt looks up the type by the unqualified name,
		// possibly because moc isn't aware of the namespace.
		qRegisterMetaType<Class>(name);
		qRegisterMetaType<Class>(qualifiedName);
	}


};

/*!
 * Declares a meta type for the given type and registers it with the Qt meta type system.
 *
 * Must be placed in the global scope.
 *
 * \param type The unqualified name of the type, e.g. AppController.
 */
#define REGISTER_META_TYPE(type)\
	Q_DECLARE_METATYPE(governikus::type)\
	static RegisterMetaType<governikus::type> _registerTypeHelper_##type(#type, "governikus::" #type);

/*!
 * Declares a meta type for the pointer type of the given type and registers it with the Qt meta type system.
 *
 * Must be placed in the global scope.
 *
 * \param type The unqualified name of the type, e.g. AppController.
 */
#define REGISTER_POINTER_META_TYPE(type)\
	Q_DECLARE_METATYPE(governikus::type*)\
	static RegisterMetaType<governikus::type*> _registerPointerTypeHelper_##type(#type "*", "governikus::" #type "*");
