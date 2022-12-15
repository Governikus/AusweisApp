/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

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
		tintColor: Style.color.accent
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
