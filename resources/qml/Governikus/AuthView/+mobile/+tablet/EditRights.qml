/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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

	signal rightsAccepted

	//: LABEL ANDROID_TABLET IOS_TABLET
	title: qsTr("Identify")

	content: Column {
		padding: Constants.pane_padding
		width: baseItem.width

		Column {
			spacing: Constants.pane_spacing
			width: parent.width - 2 * Constants.pane_padding

			GText {
				id: actionText

				//: LABEL ANDROID_TABLET IOS_TABLET
				text: qsTr("You are about to identify yourself towards the following provider:")
				width: parent.width
			}
			GPane {
				anchors {
					left: parent.left
					right: parent.right
				}
				ProviderInfoSection {
					imageSource: "qrc:///images/provider/information.svg"
					name: CertificateDescriptionModel.subjectName
					//: LABEL ANDROID_TABLET IOS_TABLET
					title: qsTr("Provider")
				}
				Item {
					height: Math.max(detailsButton.height, confirmButton.height)
					width: parent.width

					GButton {
						id: detailsButton
						Accessible.description: qsTr("Show more information about the service provider")
						activeFocusOnTab: true
						icon.source: "qrc:///images/info.svg"

						//: LABEL ANDROID_TABLET IOS_TABLET
						text: qsTr("Details about the provider")

						onClicked: push(certificateDescriptionPage)

						anchors {
							left: parent.left
							verticalCenter: parent.verticalCenter
						}
					}
					GButton {
						id: confirmButton
						icon.source: "qrc:///images/identify.svg"

						//: LABEL ANDROID_TABLET IOS_TABLET %1 can be "CAN" or "PIN"
						text: qsTr("Proceed to %1 entry").arg(NumberModel.isCanAllowedMode ?
							//: LABEL ANDROID_TABLET IOS_TABLET  Inserted into "Proceed to %1 entry"
							qsTr("CAN") :
							//: LABEL ANDROID_TABLET IOS_TABLET Inserted into "Proceed to %1 entry"
							qsTr("PIN"))
						tintIcon: true

						onClicked: rightsAccepted()

						anchors {
							right: parent.right
							verticalCenter: parent.verticalCenter
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
				text: NumberModel.isCanAllowedMode ?
				//: LABEL ANDROID_TABLET IOS_TABLET
				qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
				//: LABEL ANDROID_TABLET IOS_TABLET
				qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
				width: parent.width
			}
			GPane {

				//: LABEL ANDROID_TABLET IOS_TABLET
				title: qsTr("Transactional information")
				visible: !!workflowModel.transactionInfo || (!writeDataPane.visible && !readDataPane.visible)

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

					//: LABEL ANDROID_TABLET IOS_TABLET
					text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
					textStyle: Style.text.normal
					visible: !writeDataPane.visible && !readDataPane.visible
					width: parent.width
				}
			}
			Row {
				id: requestedDataRow

				readonly property int columnWidth: (width - spacing) / maxColumns
				readonly property int maxColumns: 3

				height: Math.max(writeDataPane.implicitHeight, readDataPane.implicitHeight)
				spacing: Constants.pane_spacing

				anchors {
					left: parent.left
					right: parent.right
				}
				GPane {
					id: writeDataPane
					height: parent.height
					visible: writeData.count > 0
					width: readDataPane.visible ? requestedDataRow.columnWidth : parent.width

					DataGroup {
						id: writeData
						chat: ChatModel.write
						columns: readDataPane.visible ? 1 : requestedDataRow.maxColumns

						//: LABEL ANDROID_TABLET IOS_TABLET
						title: qsTr("Write access (update)")
						titleStyle: Style.text.header_warning
						width: parent.width

						onScrollPageDown: baseItem.scrollPageDown()
						onScrollPageUp: baseItem.scrollPageUp()
					}
				}
				GPane {
					id: readDataPane
					height: parent.height
					visible: requiredData.count > 0 || optionalData.count > 0
					width: writeDataPane.visible ? requestedDataRow.columnWidth * 2 : parent.width

					Row {
						spacing: Constants.pane_spacing
						width: parent.width

						DataGroup {
							id: requiredData
							chat: ChatModel.required
							columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))

							//: LABEL ANDROID_TABLET IOS_TABLET
							title: qsTr("Read access")
							width: optionalData.visible ? parent.width / 2 : parent.width

							onScrollPageDown: baseItem.scrollPageDown()
							onScrollPageUp: baseItem.scrollPageUp()
						}
						DataGroup {
							id: optionalData
							chat: ChatModel.optional
							columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))

							//: LABEL ANDROID_TABLET IOS_TABLET
							title: qsTr("Read access (optional)")
							width: requiredData.visible ? parent.width / 2 : parent.width

							onScrollPageDown: baseItem.scrollPageDown()
							onScrollPageUp: baseItem.scrollPageUp()
						}
					}
				}
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: workflowModel.cancelWorkflow()
	}
}
