/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

QtObject {
	readonly property var button: TextStyle {
		fontWeight: Style.font.normal
		lineHeight: Style.dimens.lineHeight_button
		textColor: Style.color.control.content.basic_unchecked
	}
	readonly property var headline: TextStyle {
		fontWeight: Style.font.medium
		lineHeight: Style.dimens.lineHeight_headline
		textColor: Style.color.textHeadline.basic_unchecked
		textSize: Style.dimens.textHeadline
	}
	readonly property var link: TextStyle {
		fontWeight: Style.font.normal
		textColor: Style.color.linkBasic.basic_unchecked
	}
	readonly property var navigation: TextStyle {
		fontWeight: Style.is_layout_desktop ? Style.font.medium : Style.font.normal
		lineHeight: Style.dimens.lineHeight_navigation
		textColor: Style.color.linkNavigation.basic_unchecked
		textSize: Style.dimens.text_navigation
	}
	readonly property var normal: TextStyle {
		fontWeight: Style.font.normal
	}
	readonly property var subline: TextStyle {
		fontWeight: Style.font.medium
		lineHeight: Style.dimens.lineHeight_subline
		textColor: Style.color.textSubline.basic_unchecked
		textSize: Style.dimens.textSubline
	}
	readonly property var tile: TextStyle {
		fontWeight: Style.font.medium
		lineHeight: Style.dimens.lineHeight_tile
		textColor: Style.color.textTitle.basic_unchecked
		textSize: Style.dimens.text_tile
	}
	readonly property var title: TextStyle {
		fontWeight: Style.font.bold
		lineHeight: Style.dimens.lineHeight_title
		textColor: Style.color.textTitle.basic_unchecked
		textSize: Style.dimens.textTitle
	}
}
