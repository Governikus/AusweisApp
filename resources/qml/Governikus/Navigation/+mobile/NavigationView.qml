/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.WorkflowModel 1.0

Item {
	id: content
	Component.onCompleted: if (!WorkflowModel.isSmartSupported) {
		navModel.set(1, {
				"image": "qrc:///images/material_history.svg",
				"desc": QT_TR_NOOP("History"),
				"module": UiModule.HISTORY
			});
	}

	ListModel {
		id: navModel
		ListElement {
			desc: QT_TR_NOOP("Start")
			image: "qrc:///images/mobile/material_home.svg"
			module: UiModule.DEFAULT
		}
		ListElement {
			desc: QT_TR_NOOP("Provider")
			image: "qrc:///images/provider.svg"
			module: UiModule.PROVIDER
		}
		ListElement {
			desc: QT_TR_NOOP("Remote")
			image: "qrc:///images/mobile/platform_specific_phone.svg"
			module: UiModule.REMOTE_SERVICE
		}
		ListElement {
			desc: QT_TR_NOOP("Settings")
			image: "qrc:///images/material_settings.svg"
			module: UiModule.SETTINGS
		}
		ListElement {
			desc: QT_TR_NOOP("Help")
			image: "qrc:///images/material_help.svg"
			module: UiModule.HELP
		}
	}
	GSeparator {
		id: topBorderLine
		width: parent.width
	}
	RowLayout {
		anchors {
			bottom: parent.bottom
			bottomMargin: Constants.is_layout_android ? Style.dimens.navigation_bar_padding : Style.dimens.navigation_bar_text_padding
			left: parent.left
			leftMargin: Style.dimens.navigation_bar_padding
			right: parent.right
			rightMargin: Style.dimens.navigation_bar_padding
			top: topBorderLine.bottom
			topMargin: Style.dimens.navigation_bar_padding
		}
		Repeater {
			id: repeater
			model: navModel

			delegate: NavigationItem {
				readonly property var mainViewSubViews: {
					let subViews = [UiModule.IDENTIFY, UiModule.SELF_AUTHENTICATION, UiModule.PINMANAGEMENT, UiModule.CHECK_ID_CARD];
					subViews.push(WorkflowModel.isSmartSupported ? UiModule.SMART : UiModule.PROVIDER);
					return subViews;
				}

				Accessible.ignored: content.Accessible.ignored
				Layout.fillHeight: true
				Layout.fillWidth: true
				selected: baseItem.activeModule === module || (module === UiModule.DEFAULT && mainViewSubViews.includes(baseItem.activeModule))
				source: image
				text: qsTr(desc)

				onClicked: {
					baseItem.resetContentArea();
					baseItem.show(module);
				}
			}
		}
	}
}
