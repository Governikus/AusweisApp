/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Definition of all logging categories.
 *
 * \see Q_LOGGING_CATEGORY
 */

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(card, "card")
Q_LOGGING_CATEGORY(card_pcsc, "card_pcsc")
Q_LOGGING_CATEGORY(card_nfc, "card_nfc")
Q_LOGGING_CATEGORY(card_remote, "card_remote")
Q_LOGGING_CATEGORY(card_smart, "card_smart")
Q_LOGGING_CATEGORY(ifd, "ifd")
Q_LOGGING_CATEGORY(card_drivers, "card_drivers")
Q_LOGGING_CATEGORY(statemachine, "statemachine")
Q_LOGGING_CATEGORY(paos, "paos")
Q_LOGGING_CATEGORY(gui, "gui")
Q_LOGGING_CATEGORY(language, "language")
Q_LOGGING_CATEGORY(qml, "qml")
Q_LOGGING_CATEGORY(json, "json")
Q_LOGGING_CATEGORY(aidl, "aidl")
Q_LOGGING_CATEGORY(websocket, "websocket")
Q_LOGGING_CATEGORY(webservice, "webservice")
Q_LOGGING_CATEGORY(scheme, "scheme")
Q_LOGGING_CATEGORY(automatic, "automatic")
Q_LOGGING_CATEGORY(cli, "cli")
Q_LOGGING_CATEGORY(stdinput, "stdinput")
Q_LOGGING_CATEGORY(fileprovider, "fileprovider")
Q_LOGGING_CATEGORY(diagnosis, "diagnosis")
Q_LOGGING_CATEGORY(rproxy, "rproxy")

/*!
 * Logging category for all security relevant messages,
 * i.e. secure data used in PACE and SecureMessaging
 */
Q_LOGGING_CATEGORY(secure, "secure", QtFatalMsg)

/*!
 * Logging category for network stuff written on our own.
 * I. e. unfortunately we do not log network traffic handled by third party libraries
 */
Q_LOGGING_CATEGORY(network, "network")

/*!
 * Logging category for the secure storage module
 */
Q_LOGGING_CATEGORY(securestorage, "securestorage")

/*!
 * Logging category for the configuration module
 */
Q_LOGGING_CATEGORY(configuration, "configuration")

/*!
 * Logging category for systems calls / signals
 */
Q_LOGGING_CATEGORY(system, "system")

/*!
 * Logging category for initial stuff like main function
 */
Q_LOGGING_CATEGORY(init, "init")

/*!
 * Logging category for services that update various application settings
 */
Q_LOGGING_CATEGORY(update, "update")

/*!
 * Logging category for app update information
 */
Q_LOGGING_CATEGORY(appupdate, "appupdate")

/*!
 * Logging category for support stuff, i.e. distinctive workflow states and workflow results needed by the support team
 */
Q_LOGGING_CATEGORY(support, "support")

/*!
 * Logging category for normally critical errors which are allowed to occur in developer mode
 */
Q_LOGGING_CATEGORY(developermode, "developermode")

/*!
 * Logging category for persistent settings
 */
Q_LOGGING_CATEGORY(settings, "settings")

/*!
 * Logging category for ApplicationModel::showFeedback
 */
Q_LOGGING_CATEGORY(feedback, "feedback")
