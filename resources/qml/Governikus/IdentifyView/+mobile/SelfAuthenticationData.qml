/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.SelfAuthModel 1.0

SectionPage {
	id: root

	navigationAction: NavigationAction { state: "cancel"; onClicked: root.done() }
	//: LABEL ANDROID IOS
	title: qsTr("Identify")

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
				height: resultIcon.height
				width: resultIcon.width + Constants.component_spacing + successText.width
				anchors.horizontalCenter: parent.horizontalCenter

				StatusIcon {
					id: resultIcon
					height: Style.dimens.medium_icon_size
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
					text: qsTr("Read data successfully")
					textStyle: Constants.is_tablet ? Style.text.header_accent : Style.text.normal_accent
				}
			}

			GPane {
				id: pane

				anchors {
					left: parent.left
					right: parent.right
					leftMargin: Constants.component_spacing
					rightMargin: Constants.component_spacing
				}

				Grid {
					id: grid
					width: parent.width
					columns: Constants.is_tablet ? 3 : 1
					spacing: Constants.text_spacing
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
		text: qsTr("OK")
		onClicked: root.done()
	}
}
