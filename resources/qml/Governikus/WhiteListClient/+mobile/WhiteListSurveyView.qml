import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: root
	leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: root.done(false) }
	headerTitleBarAction: TitleBarAction { text: qsTr("Feedback") + settingsModel.translationTrigger; font.bold: true }

	signal done(bool pUserAccepted)

	content: Column {
		width: root.width
		padding: Constants.pane_padding
		spacing: Constants.pane_spacing

		Pane {
			id: whitePane
			anchors.margins: Constants.pane_padding
			title: qsTr("Send device data?") + settingsModel.translationTrigger

			Text {
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Would you like to help us to improve the AusweisApp2?") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				color: Constants.secondary_text
				wrapMode: Text.WordWrap
			}
			Text {
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Supplying your device characteristics helps us to gather reliable information about the compatibility of your device.") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				color: Constants.secondary_text
				wrapMode: Text.WordWrap
			}
			Text {
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("The transmission is anonymous. No personal data is collected or transmitted!") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				color: Constants.secondary_text
				wrapMode: Text.WordWrap
			}
			Text {
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("The following information will be transmitted, if you decide so:") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				color: Constants.secondary_text
				wrapMode: Text.WordWrap
			}

			Item {
				anchors.left: parent.left
				anchors.right: parent.right
				height: Math.max(column1.height, column2.height)

				ListModel {
					id: leftInformationModel
					ListElement {
						entry: QT_TR_NOOP("Vendor")
					}
					ListElement {
						entry: QT_TR_NOOP("Model number")
					}
					ListElement {
						entry: QT_TR_NOOP("Model name")
					}
					ListElement {
						entry: QT_TR_NOOP("Collection date")
					}
					ListElement {
						entry: QT_TR_NOOP("AusweisApp2 version")
					}
				}

				ListModel {
					id: rightInformationModel
					ListElement {
						entry: QT_TR_NOOP("ROM build number")
					}
					ListElement {
						entry: QT_TR_NOOP("Android version")
					}
					ListElement {
						entry: QT_TR_NOOP("Kernel version")
					}
					ListElement {
						entry: QT_TR_NOOP("Max. NFC packet length")
					}
				}

				Column {
					id: column1
					width: parent.width / 2
					anchors.left: parent.left

					Repeater {
						model: leftInformationModel
						delegate: BulletPointDelegate {
							text: qsTr(entry) + settingsModel.translationTrigger
						}
					}
				}

				Column {
					id: column2
					width: parent.width / 2
					anchors.right: parent.right

					Repeater {
						model: rightInformationModel
						delegate: BulletPointDelegate {
							text: qsTr(entry) + settingsModel.translationTrigger
						}
					}
				}
			}

			Text {
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Thank you for your assistance!") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.secondary_text
				wrapMode: Text.WordWrap
			}
		}


		Row {
			spacing: Constants.component_spacing
			height: childrenRect.height
			anchors.horizontalCenter: parent.horizontalCenter

			GButton {
				text: qsTr("Cancel") + settingsModel.translationTrigger
				onClicked: root.done(false)
			}

			GButton {
				text: qsTr("Transmit") + settingsModel.translationTrigger
				onClicked: root.done(true)
			}
		}
	}
}
