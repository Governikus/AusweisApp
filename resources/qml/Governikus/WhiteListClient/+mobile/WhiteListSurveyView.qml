/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SurveyModel 1.0

SectionPage {
	id: root
	signal done(bool pUserAccepted)

	//: LABEL ANDROID IOS
	title: qsTr("Feedback")

	content: Column {
		padding: Constants.pane_padding
		spacing: Constants.pane_spacing
		width: root.width

		GPane {
			id: whitePane

			//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Header
			title: qsTr("Send device data?")

			anchors {
				left: parent.left
				margins: Constants.pane_padding
				right: parent.right
			}
			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
				text: qsTr("Would you like to help us to improve the AusweisApp2?")
			}
			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
				text: qsTr("Supplying your device characteristics helps us to gather reliable information about the compatibility of your device.")
			}
			GText {
				anchors.left: parent.left
				anchors.right: parent.right
				//: INFO ANDROID IOS Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text
				text: qsTr("The transmission is anonymous. No personal data is collected or transmitted!")
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
					anchors.left: parent.left
					anchors.right: parent.right
					height: showDataButton.height + Constants.pane_spacing

					background: Rectangle {
						color: collapsableCollectedData.down ? Style.color.tutorial_box_background : Style.color.background_pane
					}
					contentItem: Item {
						Item {
							id: showDataButton
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.top: parent.top
							height: showDataText.height

							GText {
								id: showDataTriangle
								anchors.right: parent.right
								anchors.top: parent.top
								horizontalAlignment: Text.AlignRight
								rightPadding: Constants.groupbox_spacing
								text: d.dataHidden ? "\u25BC" : "\u25B2"
							}
							GText {
								id: showDataText
								anchors.bottom: showDataTriangle.bottom
								anchors.bottomMargin: showDataText.height / 8
								anchors.right: showDataTriangle.left
								rightPadding: Constants.groupbox_spacing
								text: qsTr("Collected data")
							}
						}
						Item {
							id: collectedData

							property real openHeight: dataColumn.implicitHeight

							anchors.top: showDataButton.bottom
							clip: true
							height: openHeight
							opacity: 0
							width: parent.width

							Column {
								id: dataColumn
								anchors.left: parent.left
								topPadding: Constants.groupbox_spacing
								width: parent.width

								Repeater {
									id: repeater
									model: SurveyModel

									delegate: LabeledText {
										label: title
										text: value
										width: dataColumn.width
									}
								}
							}
						}
					}
					states: [
						State {
							name: "open"
							when: !d.dataHidden

							PropertyChanges {
								height: collectedData.openHeight + showDataButton.height + Constants.pane_spacing
								target: collapsableCollectedData
							}
							PropertyChanges {
								height: collectedData.openHeight
								target: collectedData
							}
							PropertyChanges {
								opacity: 1.0
								target: collectedData
							}
						}
					]
					transitions: [
						Transition {
							PropertyAnimation {
								duration: 500
								easing.type: Easing.InOutQuad
								property: "height"
								target: collectedData
							}
							PropertyAnimation {
								duration: 500
								easing.type: Easing.InOutQuad
								property: "opacity"
								target: collectedData
							}
							PropertyAnimation {
								duration: 500
								easing.type: Easing.InOutQuad
								property: "height"
								target: collapsableCollectedData
							}
						}
					]

					onClicked: {
						d.dataHidden = !d.dataHidden;
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
				text: qsTr("Thank you for your assistance!")
			}
		}
		Row {
			anchors.horizontalCenter: parent.horizontalCenter
			height: childrenRect.height
			spacing: Constants.component_spacing

			GButton {
				//: LABEL ANDROID IOS
				text: qsTr("Do not send")

				onClicked: root.done(false)
			}
			GButton {
				//: LABEL ANDROID IOS
				text: qsTr("Send")

				onClicked: root.done(true)
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.done(false)
	}

	QtObject {
		id: d

		property bool dataHidden: true
	}
}
