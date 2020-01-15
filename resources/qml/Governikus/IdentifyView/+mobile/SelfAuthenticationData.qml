/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: root

	navigationAction: NavigationAction { state: "cancel"; onClicked: root.done() }
	//: LABEL ANDROID IOS
	title: qsTr("Identify") + SettingsModel.translationTrigger

	signal done()

	content: Item {
		height: content.height + okButton.height + Constants.component_spacing
		width: root.width

		Column {
			id: content
			width: parent.width
			padding: Constants.component_spacing
			spacing: Constants.component_spacing

			Item {
				id: message
				height: 60
				width: resultIcon.width + Constants.component_spacing + successText.width
				anchors.horizontalCenter: parent.horizontalCenter

				StatusIcon {
					id: resultIcon
					height: parent.height
					anchors.top: parent.top
					anchors.verticalCenter: parent.verticalCenter
					source: "qrc:///images/status_ok.svg"
				}

				GText {
					id: successText
					anchors.left: resultIcon.right
					anchors.leftMargin: Constants.component_spacing
					anchors.verticalCenter: resultIcon.verticalCenter
					//: INFO ANDROID IOS The self authentication was successfully completed.
					text: qsTr("Read data successfully") + SettingsModel.translationTrigger
					textStyle: Constants.is_tablet ? Style.text.header_accent : Style.text.normal_accent
				}
			}

			Pane {
				id: pane
				anchors.leftMargin: Constants.component_spacing
				anchors.rightMargin: Constants.component_spacing

				Grid {
					id: grid
					width: parent.width
					columns: Constants.is_tablet ? 3 : 1
					spacing: 15
					verticalItemAlignment: Grid.AlignBottom
					Repeater {
						model: SelfAuthModel

						LabeledText {
							width: (pane.width - 2 * Constants.pane_padding - (grid.columns - 1) * grid.spacing) / grid.columns

							Accessible.onScrollDownAction: root.scrollPageDown()
							Accessible.onScrollUpAction: root.scrollPageUp()

							label: name
							text: value === "" ? "---" : value
						}
					}
				}
			}
		}
	}

	GButton {
		id: okButton
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Constants.component_spacing
		//: LABEL ANDROID IOS
		text: qsTr("OK") + SettingsModel.translationTrigger
		onClicked: root.done()
	}
}
