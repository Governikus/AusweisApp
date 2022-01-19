/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0

Item {
	id: baseItem

	QtObject {
		id: d

		property int testPkiCounter: 0

		function toggleTestPki() {
			d.testPkiCounter += 1
			switch (d.testPkiCounter) {
				case 7:
				case 8:
				case 9:
					//: INFO ANDROID IOS Used in notifications when the user taps the magnifying glass
					ApplicationModel.showFeedback(qsTr("%1 more presses to toggle the testmode for the self-authentication.").arg(10 - d.testPkiCounter), true)
					break;
				case 10:
					SettingsModel.useSelfauthenticationTestUri = !SettingsModel.useSelfauthenticationTestUri
					if (SettingsModel.useSelfauthenticationTestUri) {
						//: INFO ANDROID IOS Used in notifications when the user taps the magnifying glass
						ApplicationModel.showFeedback(qsTr("Testmode for the self-authentication activated."), true)
					} else {
						//: INFO ANDROID IOS Used in notifications when the user taps the magnifying glass
						ApplicationModel.showFeedback(qsTr("Testmode for the self-authentication deactivated."), true)
					}
					d.testPkiCounter = 0;
					break;
			}
		}
	}

	Column {
		readonly property int maxWidth: width - 2 * Constants.pane_padding
		width: baseItem.width

		id: root
		spacing: Constants.component_spacing
		padding: Constants.pane_padding

		Item {
			height: childrenRect.height
			width: root.maxWidth

			Image {
				readonly property double ratio: 1080 / 815
				readonly property int maxHeight: baseItem.height - 4 * Constants.component_spacing - textPane.height - selfAuthButton.height

				id: useNpa
				anchors.left: parent.left
				width: Math.min(maxHeight * ratio, parent.width * 0.4)
				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/siteWithLogo.png"

				MouseArea {
					anchors.fill: parent
					onClicked: d.toggleTestPki()
				}
			}

			GText {
				anchors.verticalCenter: useNpa.verticalCenter
				anchors.leftMargin: Constants.component_spacing
				anchors.left: useNpa.right
				anchors.right: parent.right
				//: LABEL ANDROID IOS
				text: qsTr("You can use your ID card anywhere you see this logo.")
			}
		}

		GPane {
			id: textPane

			anchors {
				left: parent.left
				right: parent.right
				leftMargin: Constants.pane_padding
				rightMargin: Constants.pane_padding
			}

			GText {
				width: parent.width

				wrapMode: Text.WordWrap
				//: LABEL ANDROID IOS
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.")
					.arg(Qt.application.name)
			}

			GText {
				readonly property string privacyStatementUrl: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)
				//: LABEL ANDROID IOS Text of the html link inside of a sentence
				readonly property string privacyStatementDescription: qsTr("data privacy statement")
				readonly property string privacyStatementLink: "<a href=\"%1\">%2</a>".arg(privacyStatementUrl).arg(privacyStatementDescription)

				width: parent.width

				Accessible.role: Accessible.Link
				Accessible.name: ApplicationModel.stripHtmlTags(text)
				Accessible.description: qsTr("Click to open link to data privacy statement in browser: %1").arg(privacyStatementUrl)
				Accessible.onPressAction: Qt.openUrlExternally(privacyStatementUrl)

				wrapMode: Text.WordWrap
				font.bold: true
				//: LABEL ANDROID IOS
				text: qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
						.arg(privacyStatementLink)
			}
		}
	}

	GButton {
		id: selfAuthButton
		icon.source: "qrc:///images/identify.svg"
		anchors.bottom: baseItem.bottom
		anchors.bottomMargin: Constants.pane_padding
		anchors.horizontalCenter: parent.horizontalCenter
		buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.warning_text : Style.color.accent
		//: LABEL ANDROID IOS
		text: qsTr("See my personal data")
		tintIcon: true

		enabled: ApplicationModel.currentWorkflow !== "authentication"
		onClicked: SelfAuthModel.startWorkflow()
	}
}
