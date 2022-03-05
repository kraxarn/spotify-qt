#pragma once

#include "dialog/base.hpp"
#include "enum/duplicatetracksresponse.hpp"

namespace Dialog
{
	class DuplicatePlaylistTrack: public Base
	{
	public:
		explicit DuplicatePlaylistTrack(QWidget *parent);

		auto response() const -> DuplicateTrackResponse;

	protected:
		void onCancel(bool checked) override;

	private:
		DuplicateTrackResponse action;

		void onAddMissingClicked(bool checked);
		void onAddAllClicked(bool checked);
	};
}
