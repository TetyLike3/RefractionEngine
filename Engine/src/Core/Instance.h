#pragma once

#include <Core/Common.h>
#include <Core/LayerSystem.h>
#include <Platform/AWindow.h>
#include <Platform/ARenderingAPI.h>
#include <Rendering/RenderLayer.h>
#include <Physics/PhysicsLayer.h>
#include <Interface/Project.h>

namespace Refraction::Engine {
	class Instance {
	public:
		Instance();
		virtual ~Instance() = default;
		void Start();
		void End();

	protected:
		Common::Shared<Platform::AWindow> mWindow;
		Common::Shared<LayerStack> mLayerStack;
		Common::Shared<Platform::ARenderingAPI> mRenderingAPI;
		Common::Shared<RenderLayer> mRenderLayer;
		Common::Shared<PhysicsLayer> mPhysicsLayer;
		Common::Shared<Project> mProjectInstance;
	};
}
