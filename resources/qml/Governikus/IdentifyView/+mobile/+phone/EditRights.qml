/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.CertificateDescriptionModel 1.0
import Governikus.Type.ChatModel 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction {
		state: "cancel"
		onClicked: AuthModel.cancelWorkflow()
	}
	//: LABEL IOS_PHONE ANDROID_PHONE
	title: qsTr("Identify")

	content: Column {
		width: baseItem.width
		padding: Constants.pane_padding

		Column {
			width: parent.width - 2 * Constants.pane_padding

			spacing: Constants.component_spacing

			GText {
				width: parent.width

				//: LABEL IOS_PHONE ANDROID_PHONE
				text: qsTr("You are about to identify yourself towards the following provider:")
				textStyle: Style.text.normal_secondary
			}

			GPane {
				anchors {
					left: parent.left
					right: parent.right
				}

				color: mouseArea.pressed ? Style.color.background_pane_active : Style.color.background_pane

				Item {
					width: parent.width
					height: providerEntries.height

					Accessible.description: qsTr("Click for more information about the provider")
					Accessible.onPressAction: mouseArea.clicked(null)

					Column {
						id: providerEntries
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.right: forwardAction.left
						spacing: Constants.pane_spacing

						ProviderInfoSection {
							imageSource: "qrc:///images/provider/information.svg"
							//: LABEL IOS_PHONE ANDROID_PHONE
							title: qsTr("Provider")
							name: CertificateDescriptionModel.subjectName
						}
						ProviderInfoSection {
							imageSource: "qrc:///images/provider/purpose.svg"
							//: LABEL IOS_PHONE ANDROID_PHONE
							title: qsTr("Purpose for reading out requested data")
							name: CertificateDescriptionModel.purpose
						}
					}

					TintableIcon {
						id: forwardAction

						anchors.right: parent.right
						anchors.verticalCenter: providerEntries.verticalCenter

						sourceSize.height: Style.dimens.small_icon_size
						source: "qrc:///images/mobile/material_arrow_right.svg"
						tintColor: Style.color.secondary_text
					}

					MouseArea {
						id: mouseArea

						anchors.fill: parent

						onClicked: firePush(certificateDescriptionPage)
					}

					Component {
						id: certificateDescriptionPage

						CertificateDescriptionPage {
							name: CertificateDescriptionModel.subjectName
						}
					}
				}
			}

			GButton {
				anchors.horizontalCenter: parent.horizontalCenter
				//: LABEL IOS_PHONE ANDROID_PHONE %1 can be "CAN" or "PIN"
				text: qsTr("Proceed to %1 entry").arg(
														NumberModel.isCanAllowedMode ?
														//: LABEL IOS_PHONE Inserted into "Proceed to %1 entry"
														qsTr("CAN") :
														//: LABEL IOS_PHONE Inserted into "Proceed to %1 entry"
														qsTr("PIN")
													)

				icon.source: "qrc:///images/identify.svg"
				tintIcon: true

				onClicked: {
					ChatModel.transferAccessRights()
					AuthModel.continueWorkflow()
				}
			}

			GText {
				width: parent.width

				text: NumberModel.isCanAllowedMode
					  //: LABEL IOS_PHONE ANDROID_PHONE
					  ? qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:")
					  //: LABEL IOS_PHONE ANDROID_PHONE
					  : qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
				textStyle: Style.text.normal_secondary
			}

			GPane {
				id: transactionInfo

				anchors {
					left: parent.left
					right: parent.right
				}

				//: LABEL IOS_PHONE ANDROID_PHONE
				title: qsTr("Transactional information")
				visible: !!transactionInfoText.text

				GText {
					id: transactionInfoText

					width: parent.width

					text: AuthModel.transactionInfo
					textStyle: Style.text.normal_secondary
				}
			}

			GPane {
				visible: !writeData.visible && !requiredData.visible && !optionalData.visible
				anchors {
					left: parent.left
					right: parent.right
				}

				GText {
					activeFocusOnTab: true

					//: LABEL DESKTOP_QML
					text: qsTr("No data requested")
					textStyle: Style.text.normal
				}
			}

			GPane {
				visible: writeData.count > 0
				anchors {
					left: parent.left
					right: parent.right
				}

				DataGroup {
					id: writeData

					width: parent.width

					onScrollPageDown: baseItem.scrollPageDown()
					onScrollPageUp: baseItem.scrollPageUp()

					//: LABEL IOS_PHONE ANDROID_PHONE
					title: qsTr("Write access (update)")
					titleStyle: Style.text.header_warning
					chat: ChatModel.write
				}
			}

			GPane {
				anchors {
					left: parent.left
					right: parent.right
				}

				DataGroup {
					id: requiredData

					width: parent.width

					onScrollPageDown: baseItem.scrollPageDown()
					onScrollPageUp: baseItem.scrollPageUp()

					//: LABEL IOS_PHONE ANDROID_PHONE
					title: qsTr("Read access")
					chat: ChatModel.required
				}

				DataGroup {
					id: optionalData

					width: parent.width

					onScrollPageDown: baseItem.scrollPageDown()
					onScrollPageUp: baseItem.scrollPageUp()

					//: LABEL IOS_PHONE ANDROID_PHONE
					title: qsTr("Read access (optional)")
					chat: ChatModel.optional
				}
			}
		}
	}
}
