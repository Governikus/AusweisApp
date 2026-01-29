/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.ResultView

ResultView {
	id: root

	animationSymbol: Symbol.Type.INFO
	animationType: AnimationLoader.Type.STATUS
	//: MOBILE
	header: qsTr("Important Notice")
	smartEidUsed: true
	text: "<style>ul{-qt-list-indent: 0;}li{margin-top:1em;}</style>" + "<ul><li>" +
	//: MOBILE
	qsTr("Do not give your smartphone to 3rd parties unattended, especially if it is already unlocked.") + "</li><li>" +
	//: MOBILE
	qsTr("Do not keep a note of the Smart-eID PIN in or on your smartphone.") + "</li><li>" +
	//: MOBILE
	qsTr("Do not give your Smart-eID PIN to 3rd parties.") + "</li><li>" +
	//: MOBILE
	qsTr("Update the operating system of your smartphone regularly.") + "</li><li>" +
	//: MOBILE
	qsTr("If your smartphone with the Smart-eID is lost or stolen, please block the Smart-eID immediately by calling the blocking hotline (+49 116 116) and providing your blocking code.") + "</li><li>" +
	//: MOBILE
	qsTr("Delete your Smart-eID before you give away or sell your smartphone.") + "</li><li>" +
	//: MOBILE
	qsTr("If you uninstall the %1 or reset your smartphone, the Smart-eID must be set up again.").arg(Qt.application.name) + "</li></ul>"
	textFormat: Text.RichText
	//: MOBILE
	title: qsTr("Set up Smart-eID")

	onCancelClicked: continueClicked()
}
