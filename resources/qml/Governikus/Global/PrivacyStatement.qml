/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

GText {
	property bool smart: false

	readonly property string privacyStatementUrl: smart
		? "https://www.ausweisapp.bund.de/%1/aa2/bmi/privacy".arg(SettingsModel.language)
		: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)

	readonly property string privacyStatementText: smart
		//: LABEL ALL_PLATFORMS
		? qsTr("By proceeding you consent to your ID card's data being read and processed by the issuing authority. Please see %1 for details.")
		//: LABEL ALL_PLATFORMS
		: qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
	readonly property string privacyStatementDescription: smart
		//: LABEL ALL_PLATFORMS Text of the Smart-eID html link inside of a sentence
		? qsTr("data privacy statement of the Federal Ministry of the Interior and Community")
		//: LABEL ALL_PLATFORMS Text of the self authentication html link inside of a sentence
		: qsTr("data privacy statement")
	readonly property string privacyStatementLink: "<a href=\"%1\">%2</a>".arg(privacyStatementUrl).arg(privacyStatementDescription)

	wrapMode: Text.WordWrap
	font.bold: true
	text: privacyStatementText.arg(privacyStatementLink)
}