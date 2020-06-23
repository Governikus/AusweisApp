/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction {
		state: "cancel"
		onClicked: AuthModel.cancelWorkflow()
	}
	//: LABEL ANDROID_PHONE
	title: qsTr("Identify") + SettingsModel.translationTrigger

	content: Column {
		width: baseItem.width
		padding: Constants.pane_padding

		Column {
			width: parent.width - 2 * Constants.pane_padding
			spacing: Constants.component_spacing

			GText {
				width: parent.width
				//: LABEL ANDROID_PHONE
				text: qsTr("You are about to identify yourself towards the following provider:") + SettingsModel.translationTrigger
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
							//: LABEL ANDROID_PHONE
							title: qsTr("Provider") + SettingsModel.translationTrigger
							name: certificateDescriptionModel.subjectName
						}
						ProviderInfoSection {
							imageSource: "qrc:///images/provider/purpose.svg"
							//: LABEL ANDROID_PHONE
							title: qsTr("Purpose for reading out requested data") + SettingsModel.translationTrigger
							name: certificateDescriptionModel.purpose
						}
					}

					Image {
						id: forwardAction
						anchors.right: parent.right
						anchors.verticalCenter: providerEntries.verticalCenter

						sourceSize.height: Style.dimens.small_icon_size
						fillMode: Image.PreserveAspectFit
						source: "qrc:///images/arrowRight.svg"

						ColorOverlay {
							anchors.fill: forwardAction
							source: forwardAction
							color: Style.color.secondary_text
						}
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
				icon.source: "qrc:///images/npa.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				//: LABEL ANDROID_PHONE %1 can be "CAN" or "PIN"
				text: qsTr("Proceed to %1 entry").arg(
														NumberModel.isCanAllowedMode ?
														//: LABEL ANDROID_PHONE Inserted into "Proceed to %1 entry"
														qsTr("CAN") :
														//: LABEL ANDROID_PHONE Inserted into "Proceed to %1 entry"
														qsTr("PIN")
													) + SettingsModel.translationTrigger
				onClicked: {
					chatModel.transferAccessRights()
					AuthModel.continueWorkflow()
				}
			}

			GText {
				width: parent.width

				text: NumberModel.isCanAllowedMode
					  //: LABEL ANDROID_PHONE
					  ? qsTr("The following data of the ID card will be transferred to the provider when you enter the CAN:")
					  //: LABEL ANDROID_PHONE
					  : qsTr("The following data of your ID card will be transferred to the provider when you enter the PIN:")
					  + SettingsModel.translationTrigger
			}

			Pane {
				width: parent.width

				Column {
					id: transactionInfo

					width: parent.width
					visible: !!transactionInfoText.text

					PaneTitle {
						height: implicitHeight * 1.5
						verticalAlignment: Text.AlignTop
						//: LABEL ANDROID_PHONE
						text: qsTr("Transactional information") + SettingsModel.translationTrigger
					}

					GText {
						id: transactionInfoText

						width: parent.width

						text: AuthModel.transactionInfo
					}
				}

				DataGroup {
					//: LABEL ANDROID_PHONE
					title: qsTr("Required Data") + SettingsModel.translationTrigger
					chat: chatModel.required
				}

				DataGroup {
					//: LABEL ANDROID_PHONE
					title: qsTr("Optional Data") + SettingsModel.translationTrigger
					chat: chatModel.optional
				}
			}
		}
	}
}
