/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View

FlickableSectionPage {
	id: root

	required property var filterModel

	spacing: Style.dimens.pane_spacing

	//: MOBILE
	title: qsTr("Filter")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	GOptionsContainer {
		Layout.fillWidth: true
		containerPadding: Style.dimens.pane_padding
		containerSpacing: Style.dimens.groupbox_spacing
		//: MOBILE
		title: qsTr("Level")

		GridLayout {
			columnSpacing: Style.dimens.groupbox_spacing
			columns: Math.max(1, (parent.width + columnSpacing) / (levelRepeater.maxItemWidth + columnSpacing))
			rowSpacing: Style.dimens.groupbox_spacing
			uniformCellWidths: true

			GRepeater {
				id: levelRepeater

				model: root.filterModel.levels

				delegate: GCheckBox {
					required property string modelData

					checked: root.filterModel.selectedLevels.indexOf(text) !== -1
					text: modelData

					onCheckedChanged: root.filterModel.configureLevel(text, checked)
				}
			}
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		containerPadding: Style.dimens.pane_padding
		containerSpacing: Style.dimens.groupbox_spacing
		//: MOBILE
		title: qsTr("Category")

		GridLayout {
			columnSpacing: Style.dimens.groupbox_spacing
			columns: Math.max(1, (parent.width + columnSpacing) / (categoryRepeater.maxItemWidth + columnSpacing))
			rowSpacing: Style.dimens.groupbox_spacing
			uniformCellWidths: true

			GRepeater {
				id: categoryRepeater

				model: root.filterModel.categories

				delegate: GCheckBox {
					required property string modelData

					checked: root.filterModel.selectedCategories.indexOf(text) !== -1
					text: modelData

					onCheckedChanged: root.filterModel.configureCategory(text, checked)
				}
			}
		}
	}
}
