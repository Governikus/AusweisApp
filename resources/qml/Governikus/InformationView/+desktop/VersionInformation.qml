/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: root

	Accessible.name: qsTr("Version information") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the version information section of the AusweisApp2.") + SettingsModel.translationTrigger

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Version information") + SettingsModel.translationTrigger
	}

	ScrollablePane {
		id: pane

		anchors.fill: root
		anchors.margins: Constants.pane_padding

		//: LABEL DESKTOP_QML
		title: qsTr("Version information") + SettingsModel.translationTrigger

		activeFocusOnTab: true

		Column {
			id: column

			spacing: Constants.component_spacing

			Repeater {
				id: repeater

				model: versionInformationModel
				delegate: LabeledText {
					id: delegate

					width: root.width

					Accessible.name: model.label + ": " + model.text
					activeFocusOnTab: true

					label: model.label
					text: model.text
				}
			}
		}
	}
}
