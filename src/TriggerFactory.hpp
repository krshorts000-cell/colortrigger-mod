#pragma once
#include "ColorTypes.hpp"
#include "ColorUtils.hpp"
#include <Geode/binding/UndoObject.hpp>

/*
 * TriggerFactory
 * --------------
 * Builds and places Color Trigger game-objects, then registers the whole batch
 * as a single undo operation so the user can Ctrl+Z the entire column at once.
 */
namespace TriggerFactory {

// Creates one Color Trigger game-object configured with the given CapturedColor.
// 'x'    – X position in level space
// 'y'    – Y position in level space
// 'fade' – Fade duration to bake into the trigger
inline GameObject* buildTrigger(LevelEditorLayer* editorLayer,
                                 const CapturedColor& cc,
                                 float x, float y, float fade) {
    auto* trigger = static_cast<GameObject*>(
        editorLayer->createObject(COLOR_TRIGGER_OBJ_ID, {x, y}, false)
    );
    if (!trigger) return nullptr;

    // Target color channel
    trigger->m_targetColorID = cc.channelID;

    // Color
    trigger->m_triggerColor = cocos2d::ccColor3B{cc.r, cc.g, cc.b};

    // Opacity (GD stores it as 0.0–1.0 in the trigger)
    trigger->m_duration = fade;
    trigger->m_opacity  = static_cast<float>(cc.opacity) / 255.f;

    // Blending
    trigger->m_blending = cc.blending;

    // Copy-color
    if (cc.copyColorID > 0) {
        trigger->m_copyColorID = cc.copyColorID;
        // HSV shift
        trigger->m_copyHSV = {cc.copyHue, cc.copySat, cc.copyBri,
                               false, false}; // s/v are not "add" mode
    }

    // Touch-triggered = off (fires when player walks into it)
    trigger->m_touchTriggered = false;

    return trigger;
}

// Generates the full column of triggers for all captured colors.
// Returns the created objects so the caller can select/register them.
// 'spawnX' – left edge X of the selection (triggers go just to the left)
// 'spawnStartY' – starting Y (column grows downward, step = -30 units)
inline std::vector<GameObject*> generateColumn(
        LevelEditorLayer* editorLayer,
        const std::vector<CapturedColor>& colors,
        float spawnX,
        float spawnStartY,
        float fade)
{
    std::vector<GameObject*> created;
    created.reserve(colors.size());

    float y = spawnStartY;
    constexpr float STEP = -30.f; // 1 block downward per trigger

    for (const auto& cc : colors) {
        auto* trig = buildTrigger(editorLayer, cc, spawnX, y, fade);
        if (trig) {
            created.push_back(trig);
            y += STEP;
        }
    }
    return created;
}

// Registers a batch of objects as a single undoable "place" action.
inline void registerUndo(LevelEditorLayer* editorLayer,
                         const std::vector<GameObject*>& objects) {
    if (objects.empty()) return;

    // UndoObject::create wraps a CCArray of objects
    auto* arr = cocos2d::CCArray::create();
    for (auto* obj : objects) arr->addObject(obj);

    auto* undo = UndoObject::create(arr, UndoCommandType::DeleteObjects);
    editorLayer->m_undoObjects->addObject(undo);
}

} // namespace TriggerFactory
