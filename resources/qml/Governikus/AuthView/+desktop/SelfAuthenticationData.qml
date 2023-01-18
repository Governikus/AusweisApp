/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem
	titleBarAction: TitleBarAction {
		rootEnabled: false
		showHelp: false
		//: LABEL DESKTOP Title of the self authentication result data view
		text: qsTr("Read self-authentication data")
	}

	Keys.onEnterPressed: okButton.onClicked()
	Keys.onEscapePressed: okButton.onClicked()
	Keys.onReturnPressed: okButton.onClicked()

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		Row {
			id: statusRow
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: baseItem.height / 4
			spacing: Constants.component_spacing

			StatusIcon {
				anchors.verticalCenter: parent.verticalCenter
				height: Style.dimens.status_icon_medium
				source: "qrc:///images/status_ok.svg"
			}
			GText {
				id: successText
				Accessible.name: successText.text
				activeFocusOnTab: true
				anchors.verticalCenter: parent.verticalCenter

				//: INFO DESKTOP Status message that the self authentication successfully completed.
				text: qsTr("Successfully read data")
				textStyle: Style.text.header

				FocusFrame {
				}
			}
		}
		Item {
			Layout.fillHeight: true
			Layout.fillWidth: true

			ScrollablePane {
				id: pane
				activeFocusOnTab: true
				height: Math.min(parent.height, implicitHeight)

				//: LABEL DESKTOP Title of the self authentication result data view
				title: qsTr("Read data")
				width: parent.width

				Grid {
					id: grid
					columns: 3
					spacing: Constants.groupbox_spacing
					verticalItemAlignment: Grid.AlignTop
					width: parent.width

					Repeater {
						id: dataRepeater
						model: SelfAuthModel

						LabeledText {
							label: name
							text: value === "" ? "---" : value
							width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns
						}
					}
				}
				RowLayout {
					anchors.right: parent.right
					spacing: Constants.component_spacing

					GButton {
						id: saveDataToPdfButton
						icon.source: "qrc:///images/desktop/material_save.svg"
						//: LABEL DESKTOP
						text: qsTr("Save as PDF...")
						tintIcon: true

						onClicked: {
							let now = new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd");
							let filenameSuggestion = "%1.%2.%3.pdf".arg(Qt.application.name).arg(qsTr("Information")).arg(now);
							fileDialog.selectFile(filenameSuggestion);
						}

						GFileDialog {
							id: fileDialog
							defaultSuffix: "pdf"
							//: LABEL DESKTOP
							nameFilters: qsTr("Portable Document Format (*.pdf)")

							//: LABEL DESKTOP
							title: qsTr("Save read self-authentication data")

							onAccepted: SelfAuthModel.exportData(file)
						}
					}
					GButton {
						id: okButton

						//: LABEL DESKTOP
						text: qsTr("OK")

						onClicked: baseItem.nextView(UiModule.DEFAULT)
					}
				}
			}
		}
	}
}
