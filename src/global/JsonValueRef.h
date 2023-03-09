/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Compatibility helper for Qt5/Qt6 JSON stuff.
 */

#pragma once

#include <QJsonValue>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
using JsonValueRef = const QJsonValueConstRef;
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
using JsonValueRef = const QJsonValueRef;
#else
using JsonValueRef = const QJsonValue&;
#endif
