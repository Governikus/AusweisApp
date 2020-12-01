/*!
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <list>
#include <QObject>

namespace governikus
{

namespace QtHooks
{
bool init();
void setPrint(bool pAdd = true, bool pRemove = true);
void print(const QObject* pObj, const char* pPrefix = " = ");
void printAlive();
const std::list<QObject*>& getQObjects();
} // namespace QtHooks

} // namespace governikus
