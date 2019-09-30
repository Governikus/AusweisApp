/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage
{
	id: root
	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	title: name

	property string name

	content: Item
	{
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column
		{
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			Pane {
				id: pane
				//: LABEL ANDROID IOS
				title: qsTr("Provider Information") + SettingsModel.translationTrigger

				Repeater {
					id: listView
					model: certificateDescriptionModel

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
}
