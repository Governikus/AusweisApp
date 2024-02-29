/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

RowLayout {
	id: baseItem

	property alias buttonIconSource: button.icon.source
	property alias buttonText: button.text
	property alias buttonTooltip: button.enabledTooltipText
	property alias description: labeledText.text
	property alias iconSource: icon.source
	property alias title: labeledText.label

	signal clicked

	spacing: Constants.component_spacing

	TintableIcon {
		id: icon

		sourceSize.width: Style.dimens.icon_size
		tintColor: Style.color.text_subline
	}
	LabeledText {
		id: labeledText

		Layout.fillWidth: true
	}
	GButton {
		id: button

		tintIcon: true

		onClicked: baseItem.clicked()
	}
}
