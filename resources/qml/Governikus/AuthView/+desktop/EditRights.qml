/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.NumberModel
import Governikus.Type.CertificateDescriptionModel
import Governikus.Type.ChatModel

SectionPage {
	id: root

	property bool detailView: false

	function showProviderInformation(show) {
		detailView = show;
		if (!detailView)
			onVisibleChanged();
		updateTitleBarActions();
	}

	Keys.onEnterPressed: event => d.onKeyboardConfirmPressed(event)
	Keys.onEscapePressed: event => {
		if (!detailView) {
			event.accepted = false;
			return;
		}
		showProviderInformation(false);
	}
	Keys.onReturnPressed: event => d.onKeyboardConfirmPressed(event)

	QtObject {
		id: d

		function onKeyboardConfirmPressed(event) {
			if (detailView) {
				showProviderInformation(false);
			} else {
				confirmButton.onClicked();
			}
		}
	}
	ColumnLayout {
		anchors.fill: parent
		anchors.topMargin: Constants.pane_padding * 2
		spacing: Constants.pane_spacing
		visible: !root.detailView

		RowLayout {
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: parent.width * 0.65
			spacing: Constants.pane_spacing

			Image {
				id: npaIcon

				source: "qrc:///images/npa.svg"
				sourceSize.height: Style.dimens.huge_icon_size
			}
			GPane {
				id: providerInfoSectionPane

				Layout.fillWidth: true

				ProviderInfoSection {
					Layout.fillWidth: true
					activeFocusOnTab: true
					image: "qrc:///images/info.svg"
					name: CertificateDescriptionModel.subjectName
					//: LABEL DESKTOP
					title: qsTr("You are about to identify yourself towards the following provider:")

					onClicked: showProviderInformation(true)
				}
			}
		}
		GButton {
			id: confirmButton

			Accessible.name: confirmButton.text
			Layout.alignment: Qt.AlignHCenter
			activeFocusOnTab: true
			icon.source: "qrc:/images/identify.svg"
			//: LABEL DESKTOP %1 can be "CAN" or "PIN"
			text: qsTr("Proceed to %1 entry").arg(NumberModel.isCanAllowedMode ?
				//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
				qsTr("CAN") :
				//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
				qsTr("PIN"))
			tintIcon: true

			onClicked: {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			}
		}
		GText {
			id: dataIntroduction

			Accessible.name: dataIntroduction.text
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			activeFocusOnTab: true
			text: NumberModel.isCanAllowedMode ?
			//: LABEL DESKTOP
			qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
			//: LABEL DESKTOP
			qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")

			FocusFrame {
			}
		}
		ScrollablePane {
			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.maximumHeight: parent.height / parent.children.length
			Layout.rightMargin: Constants.pane_padding
			activeFocusOnTab: true
			backgroundColor: Style.color.pane_sublevel
			minimumVisibleContentHeight: transactionText.effectiveFirstLineHeight

			//: LABEL DESKTOP
			title: qsTr("Transactional information")
			visible: !!AuthModel.transactionInfo || !requestedDataRow.visible

			GText {
				id: transactionText

				Accessible.name: transactionText.text
				activeFocusOnTab: true
				text: AuthModel.transactionInfo
				visible: !!text

				FocusFrame {
				}
			}
			GText {
				id: noAccessRightsText

				Accessible.name: noAccessRightsText.text
				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
				visible: !writeDataPane.visible && !readDataPane.visible

				FocusFrame {
				}
			}
		}
		RowLayout {
			id: requestedDataRow

			readonly property int maxColumns: 3

			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.maximumHeight: implicitHeight
			Layout.rightMargin: Constants.pane_padding
			spacing: Constants.pane_spacing
			visible: writeData.count > 0 || requiredData.count > 0 || optionalData.count > 0

			ScrollablePane {
				id: writeDataPane

				Layout.alignment: Qt.AlignTop
				Layout.fillHeight: true
				Layout.fillWidth: true
				backgroundColor: Style.color.pane_sublevel
				visible: writeData.count > 0

				DataGroup {
					id: writeData

					Layout.fillWidth: true
					chat: ChatModel.write
					columns: readDataPane.visible ? 1 : requestedDataRow.maxColumns

					//: LABEL DESKTOP
					title: qsTr("Write access (update)")
					titleStyle: Style.text.headline
					writeAccess: true
				}
			}
			ScrollablePane {
				id: readDataPane

				Layout.alignment: Qt.AlignTop
				Layout.fillHeight: true
				Layout.fillWidth: true
				backgroundColor: Style.color.pane_sublevel
				visible: requiredData.count > 0 || optionalData.count > 0

				RowLayout {
					id: readRow

					readonly property real columnWidth: Math.max(requiredData.implicitWidth / requiredData.columns, optionalData.implicitWidth / optionalData.columns)

					spacing: Constants.pane_spacing

					DataGroup {
						id: requiredData

						Layout.alignment: Qt.AlignTop | Qt.AlignRight
						Layout.fillWidth: true
						Layout.preferredWidth: readRow.columnWidth * columns
						chat: ChatModel.required
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))

						//: LABEL DESKTOP
						title: qsTr("Read access")
					}
					DataGroup {
						id: optionalData

						Layout.alignment: Qt.AlignTop | Qt.AlignLeft
						Layout.fillWidth: true
						Layout.preferredWidth: readRow.columnWidth * columns
						chat: ChatModel.optional
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))

						//: LABEL DESKTOP
						title: qsTr("Read access (optional)")
					}
				}
			}
		}
		GSpacer {
			Layout.fillHeight: true
			Layout.fillWidth: true
		}
	}
	CertificateDescriptionPage {
		id: certificateDescriptionPage

		visible: root.detailView
	}
}
