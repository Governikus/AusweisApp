/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Type

ConfirmationPopup {
	//: ANDROID
	cancelButtonText: qsTr("Do not ask again")
	//: ANDROID
	okButtonText: qsTr("Rate app")
	//: ANDROID Content of the app rating popup.
	text: qsTr("We would be very grateful if you could leave a rating on the Google Play Store!")
	//: ANDROID Header of the app rating popup.
	title: qsTr("Are you satisfied with %1?").arg(Qt.application.name)

	onConfirmed: Qt.openUrlExternally(ApplicationModel.storeUrl)
}
