#pragma once

enum class DialogAction: char
{
	/**
	 * No action
	 */
	None = 0,

	/**
	 * OK, save changes and dismiss dialog
	 */
	Ok = 1,

	/**
	 * Apply, save changes without dismissing dialog
	 */
	Apply = 2,

	/**
	 * Cancel, discard changes and dismiss dialog
	 */
	Cancel = 4,
};
