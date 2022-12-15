/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
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

	property alias actionText: actionText.text
	property alias dataText: dataPasswordText.text
	property var workflowModel: AuthModel

	//: LABEL IOS_PHONE ANDROID_PHONE
	title: qsTr("Identify")

	content: Column {
		padding: Constants.pane_padding
		width: baseItem.width

		Column {
			spacing: Constants.component_spacing
			width: parent.width - 2 * Constants.pane_padding

			GText {
				id: actionText

				//: LABEL IOS_PHONE ANDROID_PHONE
				text: qsTr("You are about to identify yourself towards the following provider:")
				textStyle: Style.text.normal_secondary
				width: parent.width
			}
			GPane {
				color: mouseArea.pressed ? Style.color.background_pane_active : Style.color.background_pane

				anchors {
					left: parent.left
					right: parent.right
				}
				Item {
					Accessible.description: providerInfoSection.title + ". " + providerInfoSection.name + ". " + providerInfoAction.text
					Accessible.role: Accessible.Button
					height: providerEntries.height
					width: parent.width

					Accessible.onPressAction: mouseArea.clicked(null)

					Column {
						id: providerEntries
						spacing: Constants.groupbox_spacing

						anchors {
							left: parent.left
							right: forwardAction.left
							rightMargin: Constants.text_spacing
							top: parent.top
						}
						ProviderInfoSection {
							id: providerInfoSection
							imageSource: "qrc:///images/provider/information.svg"
							name: CertificateDescriptionModel.subjectName
							//: LABEL IOS_PHONE ANDROID_PHONE
							title: qsTr("Provider")
						}
						GText {
							id: providerInfoAction
							Accessible.ignored: true

							//: LABEL IOS_PHONE ANDROID_PHONE
							text: qsTr("Touch for more details")
							textStyle: Style.text.normal_accent
							wrapMode: Text.WordWrap

							anchors {
								left: parent.left
								leftMargin: Style.dimens.icon_size + Constants.groupbox_spacing
								right: parent.right
							}
						}
					}
					TintableIcon {
						id: forwardAction
						anchors.right: parent.right
						anchors.verticalCenter: providerEntries.verticalCenter
						source: "qrc:///images/mobile/material_arrow_right.svg"
						sourceSize.height: Style.dimens.small_icon_size
						tintColor: Style.color.secondary_text
					}
					MouseArea {
						id: mouseArea
						anchors.fill: parent

						onClicked: push(certificateDescriptionPage)
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
				icon.source: "qrc:///images/identify.svg"
				//: LABEL IOS_PHONE ANDROID_PHONE %1 can be "CAN" or "PIN"
				text: qsTr("Proceed to %1 entry").arg(NumberModel.isCanAllowedMode ?
					//: LABEL IOS_PHONE Inserted into "Proceed to %1 entry"
					qsTr("CAN") :
					//: LABEL IOS_PHONE Inserted into "Proceed to %1 entry"
					qsTr("PIN"))
				tintIcon: true

				onClicked: {
					ChatModel.transferAccessRights();
					workflowModel.continueWorkflow();
				}
			}
			GText {
				id: dataPasswordText
				text: NumberModel.isCanAllowedMode ?
				//: LABEL IOS_PHONE ANDROID_PHONE
				qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
				//: LABEL IOS_PHONE ANDROID_PHONE
				qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
				textStyle: Style.text.normal_secondary
				width: parent.width
			}
			GPane {

				//: LABEL IOS_PHONE ANDROID_PHONE
				title: qsTr("Transactional information")
				visible: !!AuthModel.transactionInfo || (!writeData.visible && !readData.visible)

				anchors {
					left: parent.left
					right: parent.right
				}
				GText {
					activeFocusOnTab: true
					text: workflowModel.transactionInfo
					textStyle: Style.text.normal_secondary
					visible: !!text
					width: parent.width
				}
				GText {
					activeFocusOnTab: true

					//: LABEL IOS_PHONE ANDROID_PHONE
					text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
					textStyle: Style.text.normal
					visible: !writeData.visible && !readData.visible
					width: parent.width
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
					chat: ChatModel.write

					//: LABEL IOS_PHONE ANDROID_PHONE
					title: qsTr("Write access (update)")
					titleStyle: Style.text.header_warning
					width: parent.width

					onScrollPageDown: baseItem.scrollPageDown()
					onScrollPageUp: baseItem.scrollPageUp()
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
					chat: ChatModel.required

					//: LABEL IOS_PHONE ANDROID_PHONE
					title: qsTr("Read access")
					width: parent.width

					onScrollPageDown: baseItem.scrollPageDown()
					onScrollPageUp: baseItem.scrollPageUp()
				}
				DataGroup {
					id: optionalData
					chat: ChatModel.optional

					//: LABEL IOS_PHONE ANDROID_PHONE
					title: qsTr("Read access (optional)")
					width: parent.width

					onScrollPageDown: baseItem.scrollPageDown()
					onScrollPageUp: baseItem.scrollPageUp()
				}
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: workflowModel.cancelWorkflow()
	}
}
