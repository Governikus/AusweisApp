import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0

SectionPage {
	id: baseItem

	leftTitleBarAction: TitleBarAction {
			state: "cancel"
			onClicked: AuthModel.cancelWorkflow()
		}
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }

	content: Column {
			width: baseItem.width
			padding: Constants.pane_padding

			Column {
				width: parent.width - 2 * Constants.pane_padding
			spacing: Constants.component_spacing

			Text {
				color: Constants.secondary_text
				font.pixelSize: Constants.normal_font_size
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("You are about to identify yourself towards the following service provider:") + settingsModel.translationTrigger
			}

			Pane {

				Item {
					width: parent.width
					height: providerEntries.height

					Column {
						id: providerEntries
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.right: forwardAction.left
						spacing: Constants.pane_spacing

						ProviderInfoSection {
							imageSource: "qrc:///images/provider/information.svg"
							title: qsTr("Service provider") + settingsModel.translationTrigger
							name: certificateDescriptionModel.subjectName
						}
						ProviderInfoSection {
							imageSource: "qrc:///images/provider/purpose.svg"
							title: qsTr("Purpose for reading out requested data") + settingsModel.translationTrigger
							name: certificateDescriptionModel.purpose
						}
					}

					Text {
						id: forwardAction
						anchors.right: parent.right
						anchors.verticalCenter: providerEntries.verticalCenter

						text: ">"
						font.pixelSize: Utils.dp(22)
						color: Constants.grey
					}

					MouseArea {
						anchors.fill: parent
						onClicked: firePush(certificateDescriptionPage)
					}

					CertificateDescriptionPage {
						id: certificateDescriptionPage
						name: certificateDescriptionModel.subjectName
						visible: false
					}
				}
			}

			GButton {
				iconSource: "qrc:///images/npa.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Identify now") + settingsModel.translationTrigger;
				onClicked: {
					chatModel.transferAccessRights()
					AuthModel.continueWorkflow()
				}
			}

			Text {
				color: Constants.secondary_text
				font.pixelSize: Constants.normal_font_size
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("The following data will be transferred to the service provider when you enter the PIN:") + settingsModel.translationTrigger
			}

			Pane {
				Column {
					height: childrenRect.height
					width: parent.width
					spacing: Utils.dp(30)

					Column {
						id: transactionInfo

						width: parent.width
						visible: !!transactionInfoText.text

						PaneTitle {
							height: implicitHeight * 1.5
							verticalAlignment: Text.AlignTop
							text: qsTr("Transactional information") + settingsModel.translationTrigger
						}

						Text {
							id: transactionInfoText
							color: Constants.secondary_text

							width: parent.width
							font.pixelSize: Constants.normal_font_size
							text: AuthModel.transactionInfo
							wrapMode: Text.WordWrap
						}
					}

					Row {
						width: parent.width
						spacing: Constants.pane_spacing

						DataGroup {
							id: requiredData
							width: optionalData.visible ? parent.width * 0.63 : parent.width

							title: qsTr("Required Data") + settingsModel.translationTrigger
							columns: optionalData.visible ? 2 : 3
							chat: chatModel.required
						}

						DataGroup {
							id: optionalData
							width: parent.width * 0.37 - Constants.pane_spacing

							title: qsTr("Optional Data") + settingsModel.translationTrigger
							chat: chatModel.optional
						}
					}
				}
			}
		}
	}
}
