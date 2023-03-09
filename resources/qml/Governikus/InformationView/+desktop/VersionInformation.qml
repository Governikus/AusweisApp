/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.VersionInformationModel 1.0

Item {
	id: baseItem

	readonly property string helpTopic: "helpVersioninformation"

	implicitHeight: column.implicitHeight
	implicitWidth: column.implicitWidth

	ColumnLayout {
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
