/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

Colors {
	id: root

	background: "#ffffff"
	border: "#bbbbbb"
	error: "#eb0000"
	focus_indicator: "#0077b6"
	image: "#0077b6"
	success: "#3e8401"
	warning: "#db6a00"

	control: DefaultControlColors {
		background.basic_checked: "#ffffff"
		background.basic_unchecked: "#0077b6"
		border.basic_checked: "#576164"
		border.basic_unchecked: "#0077b6"
		content.basic_checked: "#576164"
		content.basic_unchecked: "#ffffff"
	}
	controlCheckbox: DefaultControlColors {
		background.basic_unchecked: root.transparent
		background.pressed_checked: "#80cdec"
		background.pressed_unchecked: "#80cdec"
		border.pressed_checked: "#80cdec"
		border.pressed_unchecked: "#80cdec"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	controlOptional: DefaultControlColors {
		background.basic_unchecked: root.transparent
		background.pressed_checked: "#0077b6"
		background.pressed_unchecked: "#0077b6"
		border.basic_unchecked: "#0077b6"
		border.pressed_checked: "#0077b6"
		border.pressed_unchecked: "#0077b6"
		content.basic_unchecked: "#0077b6"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	controlRadiobutton: DefaultControlColors {
		background.hovered_checked: "#f2f3f4"
		background.hovered_unchecked: "#f2f3f4"
		border.basic_unchecked: "#bcc0c1"
		border.hovered_checked: "#bcc0c1"
		border.hovered_unchecked: "#bcc0c1"
		border.pressed_checked: "#bcc0c1"
		border.pressed_unchecked: "#bcc0c1"
		content.hovered_checked: "#576164"
		content.hovered_unchecked: "#576164"
	}
	controlScrollbar: DefaultControlColors {
		background.basic_checked: "#ffffff"
		background.basic_unchecked: "#0077b6"
		background.pressed_checked: "#80cdec"
		background.pressed_unchecked: "#80cdec"
		border.basic_checked: "#576164"
		border.basic_unchecked: "#0077b6"
		border.pressed_checked: "#80cdec"
		border.pressed_unchecked: "#80cdec"
		content.basic_checked: "#576164"
		content.basic_unchecked: "#ffffff"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	controlStagedProgressBar: DefaultControlColors {
		background.basic_unchecked: "#80cdec"
		border.basic_unchecked: root.transparent
		content.basic_unchecked: "#0077b6"
	}
	controlSwitch: DefaultControlColors {
		background.pressed_checked: "#80cdec"
		background.pressed_unchecked: "#80cdec"
		border.pressed_checked: "#80cdec"
		border.pressed_unchecked: "#80cdec"
		content.pressed_checked: "#ffffff"
		content.pressed_unchecked: "#ffffff"
	}
	linkBasic: DefaultLinkColors {
		hovered_checked: "#379ec8"
		hovered_unchecked: "#379ec8"
	}
	linkNavigation: DefaultLinkColors {
		basic_unchecked: "#004b76"
	}
	linkTitle: DefaultLinkColors {
		basic_unchecked: "#576164"
	}
	pane: DefaultPaneColors {
	}
	paneSublevel: DefaultPaneColors {
		background.basic_unchecked: "#f2f3f4"
		border.basic_unchecked: "#f2f3f4"
	}
	remoteIndicator: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#0077b6"
		disabled_checked: "#bcc0c1"
		disabled_unchecked: "#bcc0c1"
		hovered_checked: basic_unchecked
		hovered_unchecked: basic_unchecked
		pressed_checked: basic_unchecked
		pressed_unchecked: basic_unchecked
	}
	textHeadline: DefaultTextColors {
	}
	textNormal: DefaultTextColors {
		basic_checked: "#ffffff"
	}
	textSubline: DefaultTextColors {
		basic_unchecked: "#0077b6"
		disabled_checked: "#80cdec"
		disabled_unchecked: "#80cdec"
	}
	textTitle: DefaultTextColors {
		basic_checked: "#0077b6"
		basic_unchecked: "#004b76"
		disabled_checked: "#798183"
		disabled_unchecked: "#798183"
	}

	component DefaultControlColors: ControlComponents {
		background: ComponentColors {
			basic_checked: "#0077b6"
			basic_unchecked: "#ffffff"
			disabled_checked: "#bcc0c1"
			disabled_unchecked: "#bcc0c1"
			hovered_checked: "#80cdec"
			hovered_unchecked: "#80cdec"
			pressed_checked: root.transparent
			pressed_unchecked: root.transparent
		}
		border: ComponentColors {
			basic_checked: "#0077b6"
			basic_unchecked: "#576164"
			disabled_checked: "#bcc0c1"
			disabled_unchecked: "#bcc0c1"
			hovered_checked: "#80cdec"
			hovered_unchecked: "#80cdec"
			pressed_checked: "#0077b6"
			pressed_unchecked: "#0077b6"
		}
		content: ComponentColors {
			basic_checked: "#ffffff"
			basic_unchecked: "#576164"
			disabled_checked: "#ffffff"
			disabled_unchecked: "#ffffff"
			hovered_checked: "#ffffff"
			hovered_unchecked: "#ffffff"
			pressed_checked: "#0077b6"
			pressed_unchecked: "#0077b6"
		}
	}
	component DefaultLinkColors: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#0077b6"
		disabled_checked: "#bcc0c1"
		disabled_unchecked: "#bcc0c1"
		hovered_checked: "#0077b6"
		hovered_unchecked: "#0077b6"
		pressed_checked: "#80cdec"
		pressed_unchecked: "#80cdec"
	}
	component DefaultPaneColors: CoreComponents {
		background: ComponentColors {
			basic_checked: "#0077b6"
			basic_unchecked: "#ffffff"
			disabled_checked: "#f2f3f4"
			disabled_unchecked: "#f2f3f4"
			hovered_checked: root.darker(basic_unchecked, 0.05)
			hovered_unchecked: root.darker(basic_unchecked, 0.05)
			pressed_checked: root.darker(basic_unchecked, 0.10)
			pressed_unchecked: root.darker(basic_unchecked, 0.10)
		}
		border: ComponentColors {
			basic_checked: basic_unchecked
			basic_unchecked: root.software_renderer ? "#bbbbbb" : root.transparent
			disabled_checked: basic_unchecked
			disabled_unchecked: basic_unchecked
			hovered_checked: basic_unchecked
			hovered_unchecked: basic_unchecked
			pressed_checked: basic_unchecked
			pressed_unchecked: basic_unchecked
		}
	}
	component DefaultTextColors: ComponentColors {
		basic_checked: basic_unchecked
		basic_unchecked: "#576164"
		disabled_checked: "#bcc0c1"
		disabled_unchecked: "#bcc0c1"
		hovered_checked: basic_unchecked
		hovered_unchecked: basic_unchecked
		pressed_checked: basic_unchecked
		pressed_unchecked: basic_unchecked
	}
}
