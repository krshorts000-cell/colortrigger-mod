#pragma once
#include "ColorTypes.hpp"

/*
 * ColorUtils
 * ----------
 * Helpers to:
 *  1. Collect color data from GJEffectManager (live level palette).
 *  2. Collect channel IDs used by a set of objects.
 *  3. Find the first free offset so shifted channels never collide with
 *     channels already present in the level.
 */
namespace ColorUtils {

// Returns the CapturedColor for a single channel from the effect manager.
// Returns std::nullopt when the channel does not exist or is at default white.
inline std::optional<CapturedColor> captureChannel(GJEffectManager* em, int channelID) {
    if (!em) return std::nullopt;
    auto* ca = em->getColorAction(channelID);
    if (!ca) return std::nullopt;

    CapturedColor cc;
    cc.channelID  = channelID;
    cc.r          = static_cast<uint8_t>(ca->m_color.r);
    cc.g          = static_cast<uint8_t>(ca->m_color.g);
    cc.b          = static_cast<uint8_t>(ca->m_color.b);
    cc.opacity    = static_cast<uint8_t>(ca->m_opacity * 255.f);
    cc.blending   = ca->m_blending;
    cc.copyColorID = ca->m_copyColorID;
    // HSV delta fields (available on ColorAction in 2.2 bindings)
    cc.copyHue    = ca->m_copyHSV.h;
    cc.copySat    = ca->m_copyHSV.s;
    cc.copyBri    = ca->m_copyHSV.v;
    return cc;
}

// Collects channel IDs (base + detail) used by the given object list.
// Never includes 0 (unset) or the special "default white" channel.
inline std::set<int> collectUsedChannels(cocos2d::CCArray* objects) {
    std::set<int> used;
    if (!objects) return used;

    for (int i = 0; i < objects->count(); ++i) {
        auto* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (!obj) continue;

        int base   = obj->m_baseColorID;
        int detail = obj->m_detailColorID;

        auto addIfValid = [&](int id) {
            if (id > 0 && id != SystemColors::WHITE) {
                used.insert(id);
            }
        };
        addIfValid(base);
        addIfValid(detail);

        // If the object IS a Color Trigger, also note its target channel
        if (obj->m_objectID == COLOR_TRIGGER_OBJ_ID) {
            addIfValid(obj->m_targetColorID);
        }
    }
    return used;
}

// Returns all channel IDs used by any object in the level (not just selection).
inline std::set<int> collectAllUsedChannels(LevelEditorLayer* editorLayer) {
    if (!editorLayer) return {};
    return collectUsedChannels(editorLayer->m_objects);
}

// Finds the smallest offset >= 1 such that none of [1..maxCustom] shifted by
// that offset collide with channels already present in the level.
// 'alreadyUsed' is the full set of channels in the existing level.
// 'toShift'     is the set of custom channel IDs that will be shifted.
inline int findFreeOffset(const std::set<int>& alreadyUsed,
                          const std::set<int>& toShift,
                          int maxCustom = 999) {
    if (toShift.empty()) return 0;
    for (int offset = 1; offset <= 999; ++offset) {
        bool ok = true;
        for (int id : toShift) {
            int shifted = id + offset;
            if (shifted > maxCustom) { ok = false; break; }
            if (alreadyUsed.count(shifted)) { ok = false; break; }
        }
        if (ok) return offset;
    }
    return 0; // no free slot found
}

} // namespace ColorUtils
