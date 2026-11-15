#pragma once
#include "ColorTypes.hpp"
#include <Geode/ui/Notification.hpp>
#include <cmath>
#include <limits>

namespace GridSnap {

constexpr float GRID_SIZE = 30.0f;
constexpr float GRID_HALF = 15.0f;

inline float nearestGridCenter(float val) {
    return GRID_HALF + std::round((val - GRID_HALF) / GRID_SIZE) * GRID_SIZE;
}

inline bool isTriggerObject(GameObject* obj) {
    if (!obj) return false;
    if (obj->m_isTrigger) return true;
    if (obj->m_objectType == GameObjectType::Modifier) return true;
    if (obj->isColorTrigger()) return true;
    if (typeinfo_cast<EffectGameObject*>(obj) != nullptr) return true;
    return false;
}

inline GameObject* findAnchorBlock(cocos2d::CCArray* objects) {
    if (!objects || objects->count() == 0) return nullptr;

    GameObject* bestSolid = nullptr;
    float bestSolidY = std::numeric_limits<float>::max();
    float bestSolidX = std::numeric_limits<float>::max();

    GameObject* bestHazard = nullptr;
    float bestHazardY = std::numeric_limits<float>::max();
    float bestHazardX = std::numeric_limits<float>::max();

    GameObject* bestDeco = nullptr;
    float bestDecoY = std::numeric_limits<float>::max();
    float bestDecoX = std::numeric_limits<float>::max();

    for (int i = 0; i < objects->count(); ++i) {
        auto* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (!obj) continue;

        // Ignore triggers and objects below floor level
        if (isTriggerObject(obj)) continue;
        if (obj->getPositionY() < -10.0f) continue;

        float posX = obj->getPositionX();
        float posY = obj->getPositionY();

        // 1. Solid structural blocks (ID 1, or solid types)
        if (obj->m_objectType == GameObjectType::Solid || obj->m_objectID == 1) {
            if (posY < bestSolidY - 0.1f || (std::abs(posY - bestSolidY) <= 0.1f && posX < bestSolidX)) {
                bestSolidY = posY;
                bestSolidX = posX;
                bestSolid = obj;
            }
        }
        // 2. Hazards / Slopes / Breakable blocks
        else if (obj->m_objectType == GameObjectType::Hazard ||
                 obj->m_objectType == GameObjectType::Slope ||
                 obj->m_objectType == GameObjectType::Breakable) {
            if (posY < bestHazardY - 0.1f || (std::abs(posY - bestHazardY) <= 0.1f && posX < bestHazardX)) {
                bestHazardY = posY;
                bestHazardX = posX;
                bestHazard = obj;
            }
        }
        // 3. Decorative objects
        else {
            if (posY < bestDecoY - 0.1f || (std::abs(posY - bestDecoY) <= 0.1f && posX < bestDecoX)) {
                bestDecoY = posY;
                bestDecoX = posX;
                bestDeco = obj;
            }
        }
    }

    if (bestSolid) return bestSolid;
    if (bestHazard) return bestHazard;
    return bestDeco;
}

inline bool snapObjectsToGrid(EditorUI* editorUI, cocos2d::CCArray* objects, bool recordUndo = false) {
    if (!editorUI || !objects || objects->count() == 0) return false;

    auto* anchor = findAnchorBlock(objects);
    if (!anchor) return false;

    float anchorX = anchor->getPositionX();
    float anchorY = anchor->getPositionY();

    float targetX = nearestGridCenter(anchorX);
    float targetY = nearestGridCenter(anchorY);

    // Prevent structural blocks from snapping below ground line (lowest valid row is Y = 15.0f)
    if (targetY < GRID_HALF && anchorY >= 0.0f) {
        targetY = GRID_HALF;
    }

    float deltaX = targetX - anchorX;
    float deltaY = targetY - anchorY;

    // Sub-pixel threshold: if already aligned to grid within 0.001 units, do nothing
    if (std::abs(deltaX) < 0.001f && std::abs(deltaY) < 0.001f) {
        return false;
    }

    auto* editorLayer = LevelEditorLayer::get();
    if (recordUndo && editorLayer) {
        auto* undo = UndoObject::createWithTransformObjects(objects, UndoCommand::Transform);
        if (undo) {
            editorLayer->addToUndoList(undo, true);
        }
    }

    // Shift ALL objects in the group by the exact same delta
    // This preserves all relative internal sub-pixel offsets of fine deco and timing of triggers!
    cocos2d::CCPoint offset{deltaX, deltaY};
    for (int i = 0; i < objects->count(); ++i) {
        auto* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (!obj) continue;
        editorUI->moveObject(obj, offset);
    }

    editorUI->updateButtons();
    editorUI->updateObjectInfoLabel();

    return true;
}

} // namespace GridSnap
