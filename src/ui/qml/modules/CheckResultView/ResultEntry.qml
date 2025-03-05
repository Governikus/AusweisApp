/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style

RowLayout {
	id: root

	enum Type {
		IsSuccess,
		IsError,
		IsInfo,
		IsWait
	}

	property int resultType: ResultEntry.Type.IsSuccess
	property alias text: textItem.text

	Accessible.focusable: true
	Accessible.name: text
	Accessible.role: Accessible.ListItem
	Layout.alignment: Qt.AlignLeft
	Layout.fillWidth: true
	Layout.maximumWidth: Math.ceil(implicitWidth)
	spacing: Style.dimens.groupbox_spacing

	StatusAnimation {
		Layout.alignment: Qt.AlignVCenter
		sourceSize.height: Style.dimens.small_icon_size
		symbol.type: {
			switch (root.resultType) {
			case ResultEntry.Type.IsSuccess:
				return Symbol.Type.CHECK;
			case ResultEntry.Type.IsInfo:
				return Symbol.Type.INFO;
			case ResultEntry.Type.IsError:
				return Symbol.Type.ERROR;
			}
		}
		tintColor: {
			switch (root.resultType) {
			case ResultEntry.Type.IsSuccess:
				return Style.color.success;
			case ResultEntry.Type.IsInfo:
				return Style.color.textNormal.basic;
			case ResultEntry.Type.IsError:
				return Style.color.warning;
			}
		}
	}
	GText {
		id: textItem

		Accessible.ignored: true
		Layout.alignment: Qt.AlignVCenter
		elide: Text.ElideRight
	}
}
