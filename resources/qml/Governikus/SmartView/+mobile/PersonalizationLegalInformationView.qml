/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0
import Governikus.ResultView 1.0

ResultView {
	id: root
	//: LABEL ANDROID IOS
	header: qsTr("Important Notice")
	resultType: ResultView.Type.IsInfo
	text: "<style>ul{-qt-list-indent: 0;}li{margin-top:1em;}</style>" + "<ul><li>" +
	//: LABEL ANDROID IOS
	qsTr("Do not give your smartphone to third parties unattended, especially if it is already unlocked.") + "</li><li>" +
	//: LABEL ANDROID IOS
	qsTr("Do not keep a note of the Smart-eID PIN in or on your smartphone.") + "</li><li>" +
	//: LABEL ANDROID IOS
	qsTr("Do not give your Smart-eID PIN to third parties.") + "</li><li>" +
	//: LABEL ANDROID IOS
	qsTr("Update the operating system of your smartphone regularly.") + "</li><li>" +
	//: LABEL ANDROID IOS
	qsTr("If your smartphone with the Smart-eID is lost or stolen, please block the Smart-eID immediately by calling the blocking hotline (+49 116 116) and providing your blocking code.") + "</li><li>" +
	//: LABEL ANDROID IOS
	qsTr("Delete your Smart-eID before you give away or sell your smartphone.") + "</li></ul>"
	textFormat: Text.RichText
	//: LABEL ANDROID IOS
	title: qsTr("Set up Smart-eID")
	titleBarColor: Style.color.accent_smart

	onCancelClicked: continueClicked()
}
