/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SurveyModel 1.0


SectionPage {
	id: root

	signal done(bool pUserAccepted)

	navigationAction: NavigationAction { state: "cancel"; onClicked: root.done(false) }
	//: LABEL ANDROID IOS
	title: qsTr("Feedback") + SettingsModel.translationTrigger

	QtObject {
		id: d

		property bool dataHidden: true
	}

	content: Column {
		width: root.width

		padding: Constants.pane_padding
		spacing: Constants.pane_spacing

		Pane {
			id: whitePane

			anchors.margins: Constants.pane_padding
			//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Header
			title: qsTr("Send device data?") + SettingsModel.translationTrigger

			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
				text: qsTr("Would you like to help us to improve the AusweisApp2?") + SettingsModel.translationTrigger
			}

			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
				text: qsTr("Supplying your device characteristics helps us to gather reliable information about the compatibility of your device.") + SettingsModel.translationTrigger
			}

			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
				text: qsTr("The transmission is anonymous. No personal data is collected or transmitted!") + SettingsModel.translationTrigger
			}

			Column {
				anchors.left: parent.left
				anchors.right: parent.right

				spacing: 2

				GSeparator {
					anchors.left: parent.left
					anchors.right: parent.right
				}

				Button {
					id: collapsableCollectedData

					height: showDataButton.height + Constants.pane_spacing
					anchors.left: parent.left
					anchors.right: parent.right

					states: [
						State {
							name: "open";
							when: !d.dataHidden

							PropertyChanges {
								target: collapsableCollectedData;
								height: collectedData.openHeight + showDataButton.height + Constants.pane_spacing
							}
							PropertyChanges {
								target: collectedData;
								height: collectedData.openHeight
							}
							PropertyChanges {
								target: collectedData;
								opacity: 1.0
							}
						}
					]
					transitions: [
						Transition {
							PropertyAnimation {
								target: collectedData
								property: "height"
								easing.type: Easing.InOutQuad
								duration: 500
							}
							PropertyAnimation {
								target: collectedData
								property: "opacity"
								easing.type: Easing.InOutQuad
								duration: 500
							}
							PropertyAnimation {
								target: collapsableCollectedData
								property: "height"
								easing.type: Easing.InOutQuad
								duration: 500
							}
						}
					]

					background: Rectangle {
						color: collapsableCollectedData.down ? Style.color.tutorial_box_background : Style.color.background_pane
					}

					onClicked: {
						d.dataHidden = !d.dataHidden
					}

					contentItem: Item {
						Item {
							id: showDataButton

							height: showDataText.height
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.top: parent.top

							GText {
								id: showDataTriangle

								anchors.top: parent.top
								anchors.right: parent.right

								rightPadding: Constants.groupbox_spacing
								text: d.dataHidden ? "\u25BC" : "\u25B2"
								horizontalAlignment: Text.AlignRight
							}

							GText {
								id: showDataText

								anchors.right: showDataTriangle.left
								anchors.bottom: showDataTriangle.bottom
								anchors.bottomMargin: showDataText.height / 8

								rightPadding: Constants.groupbox_spacing
								text: qsTr("Collected data") + SettingsModel.translationTrigger
							}
						}

						Item {
							id: collectedData

							property real openHeight: dataColumn.implicitHeight

							height: openHeight
							width: parent.width
							anchors.top: showDataButton.bottom

							opacity: 0
							clip: true

							Column {
								id: dataColumn

								width: parent.width
								anchors.left: parent.left

								topPadding: Constants.groupbox_spacing

								Repeater {
									id: repeater

									model: SurveyModel
									delegate: LabeledText {
										width: dataColumn.width
										label: title
										text: value
									}
								}
							}
						}
					}
				}

				GSeparator {
					anchors.left: parent.left
					anchors.right: parent.right
				}
			}

			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Thank you message
				text: qsTr("Thank you for your assistance!") + SettingsModel.translationTrigger
			}
		}


		Row {
			height: childrenRect.height
			anchors.horizontalCenter: parent.horizontalCenter

			spacing: Constants.component_spacing

			GButton {
				//: LABEL ANDROID IOS
				text: qsTr("Cancel") + SettingsModel.translationTrigger
				onClicked: root.done(false)
			}

			GButton {
				//: LABEL ANDROID IOS
				text: qsTr("Transmit") + SettingsModel.translationTrigger
				onClicked: root.done(true)
			}
		}
	}
}
