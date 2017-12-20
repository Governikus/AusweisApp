import QtQuick 2.5

import Governikus.Global 1.0
import Governikus.Provider 1.0

Item {
	height: identifycolumn.height + Utils.dp(30)

	Column {
		id: identifycolumn
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.component_spacing

		spacing: Constants.component_spacing

		Text {
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
			buttonColor: applicationModel.currentWorkflow === "" && settingsModel.useSelfauthenticationTestUri ? Constants.red : Constants.blue
			text: qsTr("Identify now") + settingsModel.translationTrigger;
			onClicked: {
				if (applicationModel.currentWorkflow === "") {
					selfAuthenticationModel.startWorkflow();
				}
				else if (applicationModel.currentWorkflow === "authentication") {
					chatModel.transferAccessRights()
					numberModel.continueWorkflow()
				}
			}
		}

		Text {
			font.pixelSize: Constants.normal_font_size
			width: parent.width
			wrapMode: Text.WordWrap
			text: qsTr("The following data will be transferred to the service provider when you enter the PIN:") + settingsModel.translationTrigger
		}

		Pane {
			width: parent.width

			Column {
				id: transactionInfo

				width: parent.width
				visible: !!transactionInfoText.text

				Text {
					height: implicitHeight * 1.5
					verticalAlignment: Text.AlignTop
					text: qsTr("Transactional information") + settingsModel.translationTrigger
					color: Constants.blue
					font.pixelSize: Constants.header_font_size
					elide: Text.ElideRight
				}

				Text {
					id: transactionInfoText

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
