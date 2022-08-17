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

	navigationAction: NavigationAction {
			action: NavigationAction.Action.Cancel
			onClicked: workflowModel.cancelWorkflow()
		}
	//: LABEL ANDROID_TABLET IOS_TABLET
	title: qsTr("Identify")

	content: Column {
		width: baseItem.width
		padding: Constants.pane_padding

		Column {
			width: parent.width - 2 * Constants.pane_padding
			spacing: Constants.pane_spacing

			GText {
				id: actionText

				width: parent.width

				//: LABEL ANDROID_TABLET IOS_TABLET
				text: qsTr("You are about to identify yourself towards the following provider:")
			}

			GPane {
				anchors {
					left: parent.left
					right: parent.right
				}

				ProviderInfoSection {
					imageSource: "qrc:///images/provider/information.svg"
					//: LABEL ANDROID_TABLET IOS_TABLET
					title: qsTr("Provider")
					name: CertificateDescriptionModel.subjectName
				}

				Item {
					width: parent.width
					height: Math.max(detailsButton.height, confirmButton.height)

					GButton {
						id: detailsButton

						anchors {
							left: parent.left
							verticalCenter: parent.verticalCenter
						}

						activeFocusOnTab: true
						Accessible.description: qsTr("Show more information about the service provider")

						//: LABEL ANDROID_TABLET IOS_TABLET
						text: qsTr("Details about the provider")
						onClicked: push(certificateDescriptionPage)
						icon.source: "qrc:///images/info.svg"
					}

					GButton {
						id: confirmButton

						anchors {
							right: parent.right
							verticalCenter: parent.verticalCenter
						}

						//: LABEL ANDROID_TABLET IOS_TABLET %1 can be "CAN" or "PIN"
						text: qsTr("Proceed to %1 entry").arg(
																NumberModel.isCanAllowedMode ?
																//: LABEL ANDROID_TABLET IOS_TABLET  Inserted into "Proceed to %1 entry"
																qsTr("CAN") :
																//: LABEL ANDROID_TABLET IOS_TABLET Inserted into "Proceed to %1 entry"
																qsTr("PIN")
															)

						icon.source: "qrc:///images/identify.svg"
						tintIcon: true

						onClicked: {
							ChatModel.transferAccessRights()
							workflowModel.continueWorkflow()
						}
					}

					Component {
						id: certificateDescriptionPage

						CertificateDescriptionPage {
							name: CertificateDescriptionModel.subjectName
						}
					}
				}
			}

			GText {
				id: dataPasswordText

				width: parent.width

				text: NumberModel.isCanAllowedMode
					  //: LABEL ANDROID_TABLET IOS_TABLET
					  ? qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:")
					  //: LABEL ANDROID_TABLET IOS_TABLET
					  : qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
			}

			GPane {
				visible: !!workflowModel.transactionInfo || (!writeDataPane.visible && !readDataPane.visible)
				anchors {
					left: parent.left
					right: parent.right
				}

				//: LABEL ANDROID_TABLET IOS_TABLET
				title: qsTr("Transactional information")

				GText {
					visible: !!text
					width: parent.width

					activeFocusOnTab: true

					text: workflowModel.transactionInfo
					textStyle: Style.text.normal_secondary
				}

				GText {
					visible: !writeDataPane.visible && !readDataPane.visible
					width: parent.width

					activeFocusOnTab: true

					//: LABEL ANDROID_TABLET IOS_TABLET
					text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
					textStyle: Style.text.normal
				}
			}

			Row {
				id: requestedDataRow

				readonly property int maxColumns: 3
				readonly property int columnWidth: (width - spacing) / maxColumns

				anchors {
					left: parent.left
					right: parent.right
				}
				height: Math.max(writeDataPane.implicitHeight, readDataPane.implicitHeight)

				spacing: Constants.pane_spacing

				GPane {
					id: writeDataPane

					visible: writeData.count > 0
					width: readDataPane.visible ? requestedDataRow.columnWidth : parent.width
					height: parent.height

					DataGroup {
						id: writeData

						width: parent.width

						onScrollPageDown: baseItem.scrollPageDown()
						onScrollPageUp: baseItem.scrollPageUp()

						//: LABEL ANDROID_TABLET IOS_TABLET
						title: qsTr("Write access (update)")
						columns: readDataPane.visible ? 1 : requestedDataRow.maxColumns
						chat: ChatModel.write
						titleStyle: Style.text.header_warning
					}
				}

				GPane {
					id: readDataPane

					visible: requiredData.count > 0 || optionalData.count > 0
					width: writeDataPane.visible ? requestedDataRow.columnWidth * 2 : parent.width
					height: parent.height

					Row {
						width: parent.width

						spacing: Constants.pane_spacing

						DataGroup {
							id: requiredData

							width: optionalData.visible ? parent.width / 2 : parent.width

							onScrollPageDown: baseItem.scrollPageDown()
							onScrollPageUp: baseItem.scrollPageUp()

							//: LABEL ANDROID_TABLET IOS_TABLET
							title: qsTr("Read access")
							columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))
							chat: ChatModel.required
						}

						DataGroup {
							id: optionalData

							width: requiredData.visible ? parent.width / 2 : parent.width

							onScrollPageDown: baseItem.scrollPageDown()
							onScrollPageUp: baseItem.scrollPageUp()

							//: LABEL ANDROID_TABLET IOS_TABLET
							title: qsTr("Read access (optional)")
							columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))
							chat: ChatModel.optional
						}
					}
				}
			}
		}
	}
}
