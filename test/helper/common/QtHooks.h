/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <list>

namespace governikus
{

namespace QtHooks
{
bool init(bool pPrintXml = false);
void clear();
void setPrint(bool pAdd = true, bool pRemove = true);
void print(const QObject* pObj, const char* pPrefix, const char* pType = "qdebug");
void printAlive();
const std::list<QObject*>& getQObjects();
} // namespace QtHooks

} // namespace governikus
