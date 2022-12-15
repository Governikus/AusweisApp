/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.CertificateDescriptionModel 1.0

SectionPage {
	id: root

	property string name

	title: name

	content: Item {
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column {
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			GPane {
				id: pane
				//: LABEL ANDROID IOS
				title: qsTr("Provider Information")

				anchors {
					left: parent.left
					right: parent.right
				}
				Repeater {
					id: listView
					model: CertificateDescriptionModel

					LabeledText {
						id: delegate
						label: model.label
						text: model.text
						textFormat: Text.PlainText
						width: parent.width
					}
				}
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}
}
