/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

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
					ApplicationModel.showFeedback(qsTr("%1 more presses to toggle the testmode for the self-authentication.").arg(10 - d.testPkiCounter) + SettingsModel.translationTrigger, true)
					break;
				case 10:
					SettingsModel.useSelfauthenticationTestUri = !SettingsModel.useSelfauthenticationTestUri
					if (SettingsModel.useSelfauthenticationTestUri) {
						//: INFO ANDROID IOS Used in notifications when the user taps the magnifying glass
						ApplicationModel.showFeedback(qsTr("Testmode for the self-authentication activated.") + SettingsModel.translationTrigger, true)
					} else {
						//: INFO ANDROID IOS Used in notifications when the user taps the magnifying glass
						ApplicationModel.showFeedback(qsTr("Testmode for the self-authentication deactivated.") + SettingsModel.translationTrigger, true)
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
					onClicked: {
						d.toggleTestPki()
					}
				}
			}

			GText {
				anchors.verticalCenter: useNpa.verticalCenter
				anchors.leftMargin: Constants.component_spacing
				anchors.left: useNpa.right
				anchors.right: parent.right
				//: LABEL ANDROID IOS
				text: qsTr("You can use your ID card anywhere you see this logo.") + SettingsModel.translationTrigger
			}
		}

		Pane {
			id: textPane
			anchors.leftMargin: Constants.pane_padding
			anchors.rightMargin: Constants.pane_padding

			GText {
				id: info

				property string htmlLink: qsTr("https://www.ausweisapp.bund.de/datenschutz/") + SettingsModel.translationTrigger
				anchors.left: parent.left
				anchors.right: parent.right

				Accessible.role: Accessible.Link
				Accessible.name: ApplicationModel.stripHtmlTags(text)
				Accessible.description: qsTr("Click to open link to data privacy statement in browser: %1").arg(htmlLink) + SettingsModel.translationTrigger
				Accessible.onPressAction: Qt.openUrlExternally(htmlLink)

				wrapMode: Text.WordWrap
				//: LABEL ANDROID IOS
				text: qsTr("Use the button 'See my personal data' to display the data stored on your ID card."
							+ " An Internet connection is required to display the data.")
							+ "<br><br><b>"
							//: LABEL ANDROID IOS
							+ qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
								.arg('<a href="' + htmlLink + '">' + qsTr("data privacy statement") + '</a>')
							+ "</b>"
							+ SettingsModel.translationTrigger
			}
		}
	}

	GButton {
		id: selfAuthButton
		icon.source: "qrc:///images/npa.svg"
		anchors.bottom: baseItem.bottom
		anchors.bottomMargin: Constants.pane_padding
		anchors.horizontalCenter: parent.horizontalCenter
		buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.warning_text : Style.color.accent
		//: LABEL ANDROID IOS
		text: qsTr("See my personal data") + SettingsModel.translationTrigger

		enabled: ApplicationModel.currentWorkflow !== "authentication"
		onClicked: {
			SelfAuthModel.startWorkflow()
		}
	}
}
