/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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

						anchors {
							top: parent.top
							right: forwardAction.left
							rightMargin: Constants.text_spacing
							left: parent.left
						}

						spacing: Constants.groupbox_spacing

						ProviderInfoSection {
							imageSource: "qrc:///images/provider/information.svg"
							//: LABEL IOS_PHONE ANDROID_PHONE
							title: qsTr("Provider")
							name: CertificateDescriptionModel.subjectName
						}

						GText {
							anchors {
								right: parent.right
								left: parent.left
								leftMargin: Style.dimens.icon_size + Constants.groupbox_spacing
							}

							//: LABEL IOS_PHONE ANDROID_PHONE
							text: qsTr("Touch for more details")
							textStyle: Style.text.normal_accent
							wrapMode: Text.WordWrap
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
				visible: !!AuthModel.transactionInfo || (!writeData.visible && !readData.visible)
				anchors {
					left: parent.left
					right: parent.right
				}

				//: LABEL IOS_PHONE ANDROID_PHONE
				title: qsTr("Transactional information")

				GText {
					visible: !!text
					width: parent.width

					activeFocusOnTab: true

					text: AuthModel.transactionInfo
					textStyle: Style.text.normal_secondary
				}

				GText {
					visible: !writeData.visible && !readData.visible
					width: parent.width

					activeFocusOnTab: true

					//: LABEL IOS_PHONE ANDROID_PHONE
					text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
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
				id: readData

				visible: requiredData.count > 0 || optionalData.count > 0
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
