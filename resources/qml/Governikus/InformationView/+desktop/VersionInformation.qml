/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel
import Governikus.Type.VersionInformationModel

Item {
	id: baseItem

	implicitHeight: column.implicitHeight
	implicitWidth: column.implicitWidth

	GPane {
		id: column

		anchors.fill: parent
		spacing: Constants.component_spacing

		Repeater {
			id: repeater

			model: VersionInformationModel

			delegate: LabeledText {
				id: delegate

				Accessible.name: model.label + ": " + model.text
				label: model.label
				text: model.text
				width: baseItem.width
			}
		}
	}
	MouseArea {
		property int counter: 0

		anchors.fill: parent

		onClicked: {
			counter += 1;
			if (counter === 10) {
				SettingsModel.developerOptions = !SettingsModel.developerOptions;
				ApplicationModel.showFeedback(SettingsModel.developerOptions ?
					//: LABEL DESKTOP
					qsTr("Developer options activated.") :
					//: LABEL DESKTOP
					qsTr("Developer options deactivated."));
				counter = 0;
			}
		}
	}
}
