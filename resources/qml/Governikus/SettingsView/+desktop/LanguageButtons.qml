/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global

GridLayout {
	id: root

	signal buttonClicked

	columnSpacing: Constants.component_spacing
	rowSpacing: Constants.component_spacing

	GRepeater {
		id: repeater

		delegate: LocationButton {
			Accessible.description: model.a11yDescription
			Accessible.name: model.a11yName
			Layout.fillWidth: true
			Layout.preferredWidth: Constants.is_desktop ? repeater.maxItemWidth : -1
			image: model.image
			language: model.language
			text: model.text

			onClicked: root.buttonClicked()
		}
		model: LanguageButtonData {
		}
	}
}
