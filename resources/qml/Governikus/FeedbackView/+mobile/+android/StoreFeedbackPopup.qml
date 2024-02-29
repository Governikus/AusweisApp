/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel

ConfirmationPopup {
	//: LABEL ANDROID
	cancelButtonText: qsTr("Do not ask again")
	//: LABEL ANDROID
	okButtonText: qsTr("Rate app")
	//: INFO ANDROID Content of the app rating popup.
	text: qsTr("We would be very grateful if you could leave a rating on the Google Play Store!")
	//: INFO ANDROID Header of the app rating popup.
	title: qsTr("Are you satisfied with %1?").arg(Qt.application.name)

	onConfirmed: Qt.openUrlExternally(ApplicationModel.storeUrl)
}
