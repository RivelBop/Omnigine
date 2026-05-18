#pragma once

namespace Omni {
    class Scene {
    public:
        // If anything is initialized in the constructor, not init(), the deconstructor MUST be used!
        virtual ~Scene() = default;

        // Called when this is initially set as the current scene, anything created here should be disposed!
        virtual void init() = 0;

        // Called continuously when this is the current scene
        virtual Scene* render(float dt) = 0;

        // Called when no longer the current scene to free memory from init(), returns true if this should be freed
        virtual bool dispose() = 0;
    };
}