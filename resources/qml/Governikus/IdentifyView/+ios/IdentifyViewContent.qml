import QtQuick 2.7

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: baseItem

	leftTitleBarAction: TitleBarAction {
		state: "cancel"
		onClicked: authModel.cancelWorkflow()
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
						font.pixelSize: Utils.sp(22)
						color: Constants.grey
					}

					MouseArea {
						anchors.fill: parent
						onClicked: firePush(certificateDescriptionPage, {})
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
					numberModel.continueWorkflow()
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
				id: transactionInfo
				title: qsTr("Transactional information") + settingsModel.translationTrigger
				visible: !!transactionInfoText.text

				Text {
					id: transactionInfoText
					color: Constants.secondary_text

					width: parent.width
					font.pixelSize: Constants.normal_font_size
					text: authModel.transactionInfo
					wrapMode: Text.WordWrap
				}
			}

			DataGroup {
				title: qsTr("Required Data") + settingsModel.translationTrigger
				chat: chatModel.required
			}

			DataGroup {
				title: qsTr("Optional Data") + settingsModel.translationTrigger
				chat: chatModel.optional
			}
		}
	}
}
