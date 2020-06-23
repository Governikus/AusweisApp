/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

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
	//: LABEL ANDROID_TABLET
	title: qsTr("Identify") + SettingsModel.translationTrigger

	content: Column {
		width: baseItem.width
		padding: Constants.pane_padding

		Column {
			width: parent.width - 2 * Constants.pane_padding
			spacing: Constants.pane_spacing

			GText {
				width: parent.width

				//: LABEL ANDROID_TABLET
				text: qsTr("You are about to identify yourself towards the following provider:") + SettingsModel.translationTrigger
			}

			Pane {

				Row {
					height: providerEntries.height
					width: parent.width
					spacing: Constants.pane_spacing

					Item {
						height: providerEntries.height
						width: (parent.width - Constants.pane_spacing) / 2

						Column {
							id: providerEntries
							anchors.top: parent.top
							anchors.left: parent.left
							anchors.right: parent.right
							spacing: Constants.pane_spacing

							ProviderInfoSection {
								imageSource: "qrc:///images/provider/information.svg"
								//: LABEL ANDROID_TABLET
								title: qsTr("Provider") + SettingsModel.translationTrigger
								name: certificateDescriptionModel.subjectName
							}
							ProviderInfoSection {
								imageSource: "qrc:///images/provider/purpose.svg"
								//: LABEL ANDROID_TABLET
								title: qsTr("Purpose for reading out requested data") + SettingsModel.translationTrigger
								name: certificateDescriptionModel.purpose
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


					Item {
						height: parent.height
						width: (parent.width - Constants.pane_spacing) / 2

						GButton {
							id: button
							icon.source: "qrc:///images/npa.svg"
							anchors.horizontalCenter: parent.horizontalCenter
							anchors.verticalCenter: parent.verticalCenter
							//: LABEL ANDROID_TABLET %1 can be "CAN" or "PIN"
							text: qsTr("Proceed to %1 entry").arg(
																	NumberModel.isCanAllowedMode ?
																	//: LABEL ANDROID_TABLET Inserted into "Proceed to %1 entry"
																	qsTr("CAN") :
																	//: LABEL ANDROID_TABLET Inserted into "Proceed to %1 entry"
																	qsTr("PIN")
																) + SettingsModel.translationTrigger
							onClicked: {
								chatModel.transferAccessRights()
								AuthModel.continueWorkflow()
							}
						}
					}
				}
			}

			GText {
				width: parent.width

				text: NumberModel.isCanAllowedMode
					  //: LABEL ANDROID_TABLET
					  ? qsTr("The following data of the ID card will be transferred to the provider when you enter the CAN:")
					  //: LABEL ANDROID_TABLET
					  : qsTr("The following data of your ID card will be transferred to the provider when you enter the PIN:")
					  + SettingsModel.translationTrigger
			}

			Pane {
				Column {
					height: childrenRect.height
					width: parent.width
					spacing: 30

					Column {
						id: transactionInfo

						width: parent.width
						visible: !!transactionInfoText.text

						PaneTitle {
							height: implicitHeight * 1.5
							verticalAlignment: Text.AlignTop
							//: LABEL ANDROID_TABLET
							text: qsTr("Transactional information") + SettingsModel.translationTrigger
						}

						GText {
							id: transactionInfoText

							width: parent.width

							text: AuthModel.transactionInfo
							textStyle: Style.text.normal_secondary
						}
					}

					Row {
						width: parent.width
						spacing: Constants.pane_spacing

						DataGroup {
							id: requiredData
							width: optionalData.visible ? parent.width * 0.63 : parent.width

							//: LABEL ANDROID_TABLET
							title: qsTr("Required Data") + SettingsModel.translationTrigger
							columns: optionalData.visible ? 2 : 3
							chat: chatModel.required
						}

						DataGroup {
							id: optionalData
							width: parent.width * 0.37 - Constants.pane_spacing

							//: LABEL ANDROID_TABLET
							title: qsTr("Optional Data") + SettingsModel.translationTrigger
							chat: chatModel.optional
						}
					}
				}
			}
		}
	}
}
