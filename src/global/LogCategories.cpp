/*
 * \brief Definition of all logging categories.
 *
 * \see Q_LOGGING_CATEGORY
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include <QLoggingCategory>

/*!
 * Logging category for all card relevant messages
 */
Q_LOGGING_CATEGORY(card, "card")

/*!
 * Logging category for bluetooth
 */
Q_LOGGING_CATEGORY(bluetooth, "bluetooth")

/*!
 * Logging category for all security relevant messages,
 * i.e. secure data used in PACE and SecureMessaging
 */
Q_LOGGING_CATEGORY(secure, "secure", QtFatalMsg)

/*!
 * Logging category for all PC/SC relevant card stuff
 */
Q_LOGGING_CATEGORY(card_pcsc, "card_pcsc")

/*!
 * Logging category for all nfc relevant card stuff
 */
Q_LOGGING_CATEGORY(card_nfc, "card_nfc")

/*!
 * Logging category for all remote card relevant stuff
 */
Q_LOGGING_CATEGORY(card_remote, "card_remote")

/*!
 * Logging category for card drivers
 */
Q_LOGGING_CATEGORY(card_drivers, "card_drivers")

/*!
 * Logging category for command line options
 */
Q_LOGGING_CATEGORY(cmdline, "cmdline")

/*!
 * Logging category for the state machine and state handling
 */
Q_LOGGING_CATEGORY(statemachine, "statemachine")

/*!
 * Logging category for PAOS message handling
 */
Q_LOGGING_CATEGORY(paos, "paos")

/*!
 * Logging category for all GUI stuff
 */
Q_LOGGING_CATEGORY(gui, "gui")

/*!
 * Logging category for network stuff written on our own.
 * I. e. unfortunately we do not log network traffic handled by third party libraries
 */
Q_LOGGING_CATEGORY(network, "network")

/*!
 * Logging category for systems calls / signals
 */
Q_LOGGING_CATEGORY(system, "system")

/*!
 * Logging category for language loader
 */
Q_LOGGING_CATEGORY(language, "language")

/*!
 * Logging category for initial stuff like main function
 */
Q_LOGGING_CATEGORY(init, "init")

/*!
 * Logging category for services that update various application settings
 */
Q_LOGGING_CATEGORY(update, "update")

/*!
 * Logging category for support stuff, i.e. distinctive workflow states and workflow results needed by the support team
 */
Q_LOGGING_CATEGORY(support, "support")

/*!
 * Logging category for UIPlugInQml
 */
Q_LOGGING_CATEGORY(qml, "qml")

/*!
 * Logging category for UIPlugInJsonApi
 */
Q_LOGGING_CATEGORY(jsonapi, "jsonapi")

/*!
 * Logging category for UIPlugInAidl
 */
Q_LOGGING_CATEGORY(aidl, "aidl")

/*!
 * Logging category for UIPlugInWebSocket
 */
Q_LOGGING_CATEGORY(websocket, "websocket")

/*!
 * Logging category for UIPlugInCli
 */
Q_LOGGING_CATEGORY(cli, "cli")

/*!
 * Logging category for input received with stdin
 */
Q_LOGGING_CATEGORY(stdinput, "stdinput")

/*!
 * Logging category for activation handler
 */
Q_LOGGING_CATEGORY(activation, "activation")

/*!
 * Logging category for normally critical errors which are allowed to occur in developer mode
 */
Q_LOGGING_CATEGORY(developermode, "developermode")
