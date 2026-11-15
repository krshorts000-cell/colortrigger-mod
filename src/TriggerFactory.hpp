#pragma once
#include "ColorTypes.hpp"

namespace TriggerFactory {

inline GameObject* buildTrigger(LevelEditorLayer* editorLayer,
                                const CapturedColor& cc,
                                float x, float y, float fade) {
    if (!editorLayer) return nullptr;
    // noUndo = true because we register a batch UndoObject for the entire column
    auto* obj = editorLayer->createObject(COLOR_TRIGGER_OBJ_ID, {x, y}, true);
    if (!obj) return nullptr;

    auto* trigger = typeinfo_cast<EffectGameObject*>(obj);
    if (!trigger) {
        trigger = static_cast<EffectGameObject*>(obj);
    }

    trigger->m_targetColor = cc.channelID;
    trigger->m_triggerTargetColor = cocos2d::ccColor3B{cc.r, cc.g, cc.b};
    trigger->m_duration = fade;
    trigger->m_opacity = static_cast<float>(cc.opacity) / 255.f;
    trigger->m_usesBlending = cc.blending;

    if (cc.copyColorID > 0) {
        trigger->m_copyColorID = cc.copyColorID;
        trigger->m_hsvValue = {cc.copyHue, cc.copySat, cc.copyBri, false, false};
    }

    trigger->m_isTouchTriggered = false;
    trigger->m_isSpawnTriggered = false;
    trigger->updateSpecialColor();

    return obj;
}

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
    constexpr float STEP = -30.f;

    for (const auto& cc : colors) {
        auto* trig = buildTrigger(editorLayer, cc, spawnX, y, fade);
        if (trig) {
            created.push_back(trig);
            y += STEP;
        }
    }
    return created;
}

inline void registerUndo(LevelEditorLayer* editorLayer,
                         const std::vector<GameObject*>& objects) {
    if (objects.empty() || !editorLayer) return;

    auto* arr = cocos2d::CCArray::create();
    for (auto* obj : objects) {
        arr->addObject(obj);
    }

    // UndoCommand::Paste is used for freshly pasted/spawned objects so Ctrl+Z deletes them
    auto* undo = UndoObject::createWithArray(arr, UndoCommand::Paste);
    if (undo) {
        editorLayer->addToUndoList(undo, true);
    }
}

} // namespace TriggerFactory
