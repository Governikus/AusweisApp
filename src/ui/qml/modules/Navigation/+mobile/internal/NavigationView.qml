/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

Control {
	id: root

	required property int activeModule

	signal show(int pModule)

	Layout.fillWidth: true
	Layout.maximumWidth: Style.dimens.max_text_width
	Layout.minimumWidth: navigationRow.Layout.minimumWidth + leftPadding + rightPadding
	Layout.preferredHeight: Math.ceil(implicitHeight)
	Layout.preferredWidth: contentItem.Layout.preferredWidth + leftPadding + rightPadding
	bottomInset: -UiPluginModel.safeAreaMargins.bottom
	bottomPadding: Style.dimens.navigation_bar_bottom_padding
	horizontalPadding: Style.dimens.navigation_bar_padding
	topPadding: Style.dimens.navigation_bar_padding

	background: RoundedRectangle {
		bottomLeftCorner: false
		bottomRightCorner: false
		color: Style.color.pane.background.basic
		layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
		radius: Style.dimens.pane_radius

		layer.effect: GDropShadow {
			shadowVerticalOffset: -3
		}
	}
	contentItem: RowLayout {
		id: navigationRow

		readonly property bool horizontalIcons: width >= Layout.preferredWidth

		Layout.preferredWidth: repeater.maxItemWidth * visibleChildren.length + spacing * (visibleChildren.length - 1)

		GRepeater {
			id: repeater

			model: navModel

			delegate: NavigationItem {
				required property string desc
				required property url image
				required property int index
				readonly property var mainViewSubViews: [UiModule.IDENTIFY, UiModule.SELF_AUTHENTICATION, UiModule.PINMANAGEMENT, UiModule.CHECK_ID_CARD, UiModule.SMART_EID]
				required property int module
				//: ANDROID IOS LABEL Relative position of current navigation tab in navigation view. %1 is replaced with the current tab's index, %2 with the total count of tabs
				readonly property string tabPositionA11y: qsTr("%1 of %2").arg(index + 1).arg(repeater.count)

				Accessible.ignored: root.Accessible.ignored
				Accessible.name: {
					//: ANDROID IOS LABEL
					var a11yName = [text, qsTr("Tab"), tabPositionA11y];
					if (checked) {
						if (Qt.platform.os === "ios") {
							//: IOS Selected navigation tab.
							a11yName.unshift(qsTr("Selection"));
							//: IOS Name of a11y element of selected navigation tab.
							a11yName.unshift(qsTr("Tab bar"));
						} else {
							//: ANDROID Currently selected navigation tab of navigation view.
							a11yName.unshift(qsTr("Selected"));
						}
					}
					return a11yName.join(". ");
				}
				Layout.fillHeight: true
				Layout.fillWidth: true
				Layout.preferredWidth: repeater.maxItemWidth
				checked: root.activeModule === module || (module === UiModule.DEFAULT && mainViewSubViews.includes(root.activeModule))
				flowHorizontally: navigationRow.horizontalIcons
				source: image
				text: qsTr(desc)

				onClicked: {
					root.show(module);
				}
			}
		}
	}

	ListModel {
		id: navModel

		ListElement {
			desc: QT_TR_NOOP("Start")
			image: "qrc:///images/mobile/home.svg"
			module: UiModule.DEFAULT
		}
		ListElement {
			desc: QT_TR_NOOP("Card reader")
			image: "qrc:///images/mobile/phone_card_reader.svg"
			module: UiModule.REMOTE_SERVICE
		}
		ListElement {
			desc: QT_TR_NOOP("Settings")
			image: "qrc:///images/mobile/settings.svg"
			module: UiModule.SETTINGS
		}
		ListElement {
			desc: QT_TR_NOOP("Help")
			image: "qrc:///images/help.svg"
			module: UiModule.HELP
		}
	}
}
