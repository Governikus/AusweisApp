/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SelfAuthModel 1.0


SectionPage
{
	id: baseItem

	Accessible.name: qsTr("Self-Authentication result data view")
	Accessible.description: qsTr("This is the self-authentication result data view of the AusweisApp2.")
	Keys.onReturnPressed: okButton.onClicked()
	Keys.onEnterPressed: okButton.onClicked()
	Keys.onEscapePressed: okButton.onClicked()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML Title of the self authentication result data view
		text: qsTr("Read self-authentication data")
		rootEnabled: false
		showHelp: false
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		Row {
			id: statusRow

			Layout.preferredHeight: baseItem.height / 4
			Layout.alignment: Qt.AlignHCenter

			spacing: Constants.component_spacing

			StatusIcon {
				height: Style.dimens.status_icon_medium
				anchors.verticalCenter: parent.verticalCenter

				source: "qrc:///images/status_ok.svg"
			}

			GText {
				id: successText

				anchors.verticalCenter: parent.verticalCenter

				activeFocusOnTab: true
				Accessible.name: successText.text

				//: INFO DESKTOP_QML Status message that the self authentication successfully completed.
				text: qsTr("Successfully read data")
				textStyle: Style.text.header_inverse

				FocusFrame {}
			}
		}

		Item {
			Layout.fillWidth: true
			Layout.fillHeight: true

			ScrollablePane {
				id: pane

				width: parent.width
				height: Math.min(parent.height, implicitHeight)

				activeFocusOnTab: true

				//: LABEL DESKTOP_QML Title of the self authentication result data view
				title: qsTr("Read data")

				Grid {
					id: grid

					width: parent.width

					columns: 3
					spacing: Constants.groupbox_spacing
					verticalItemAlignment: Grid.AlignTop

					Repeater {
						id: dataRepeater
						model: SelfAuthModel

						LabeledText {
							width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns

							label: name
							text: value === "" ? "---" : value
						}
					}
				}

				RowLayout {

					anchors.right: parent.right
					spacing: Constants.component_spacing

					GButton {
						id: saveDataToPdfButton

						icon.source: "qrc:///images/desktop/material_save.svg"
						//: LABEL DESKTOP_QML
						text: qsTr("Save as PDF...")
						tintIcon: true
						onClicked: {
							let now = new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
							let filenameSuggestion = "%1.%2.%3.pdf".arg(Qt.application.name).arg(qsTr("Information")).arg(now)
							appWindow.openSaveFileDialog(SelfAuthModel.exportData, filenameSuggestion, qsTr("Portable Document Format"), "pdf")
						}
					}

					GButton {
						id: okButton

						//: LABEL DESKTOP_QML
						text: qsTr("OK")
						onClicked: baseItem.nextView(SectionPage.Views.Main)
					}

				}

			}
		}
	}
}
