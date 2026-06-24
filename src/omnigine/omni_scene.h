#pragma once

namespace Omni
{
class Scene
{
  public:
    /**
     * If anything is initialized in the constructor, not init(), the deconstructor MUST be used!
     * Should be mostly avoided for stack-allocated Scene objects.
     */
    virtual ~Scene() = default;

    /**
     * Called when this is initially set as the current scene, anything created here should be disposed!
     * Great for resetting stack-allocated Scene objects.
     */
    virtual void init() = 0;

    /**
     * Called continuously when this is the current scene.
     * Return this if current scene should stay, nullptr on exit, other Scene* for a new scene to set as current.
     */
    virtual Scene *render(float dt) = 0;

    /** Called when no longer the current scene to free memory from init(), return true if this should be freed. */
    virtual bool dispose() = 0;
};
} // namespace Omni