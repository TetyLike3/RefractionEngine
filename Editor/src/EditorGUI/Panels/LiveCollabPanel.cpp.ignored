#include <imgui/imgui.h>

#include <EditorState.h>
#include <EditorNet.h>

#include "LiveCollabPanel.h"

namespace Refraction::Editor::GUI {
	void LiveCollabPanel::OnDraw() {
		if (!EditorState::Temp.PanelLiveCollabVisible) return;
		ImGui::SetNextWindowSizeConstraints({ 150, 50 }, { FLT_MAX, FLT_MAX });
		ImGui::Begin("Collaboration", &EditorState::Temp.PanelLiveCollabVisible);
		if (EditorState::Temp.SimulatingGame) ImGui::BeginDisabled();

		auto& project = EditorState::Temp.ProjectInstance;
		if (project->IsRemote()) {
			if (ImGui::Button("Disconnect")) {
				EditorNet::Shutdown();
			}
		} else {
			// TODO: Allow user to establish server/connection
		}

		if (EditorState::Temp.SimulatingGame) ImGui::EndDisabled();
		ImGui::End();
	}
}
