/*
 * All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
 * its licensors.
 *
 * For complete copyright and license terms please see the LICENSE at the root of this
 * distribution (the "License"). All use of this software is governed by the License,
 * or, if provided, by the license below or the license accompanying this file. Do not
 * remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 */

#pragma once
#include "ImGuiManager.h"

#ifdef IMGUI_ENABLED
#include "ImGuiBus.h"
#include <AzCore/std/containers/map.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace ImGui
{
    class ImGuiLYEntityOutliner
        : public ImGuiEntityOutlinerRequestBus::Handler
    {
    public:
        ImGuiLYEntityOutliner();
        ~ImGuiLYEntityOutliner();

        // Called from owner
        void Initialize();
        void Shutdown();

        // Draw the Im Gui Menu
        void ImGuiUpdate();

        // Toggle the menu on and off
        void ToggleEnabled() { m_enabled = !m_enabled; }

        // -- ImGuiEntityOutlinerRequestBus::Handler Interface ---------------------
        void RequestEntityView(AZ::EntityId entity) override;
        void RequestComponentView(ImGuiEntComponentId component) override;
        void EnableTargetViewMode(bool enabled) override;
        void EnableComponentDebug(const AZ::TypeId& comType, int priority = 1) override;
        // -- ImGuiEntityOutlinerRequestBus::Handler Interface ---------------------

    private:
        class EntityInfoNode; // Forward declare for smart pointer type, so smart pointer type can go in struct
        // We should use smart pointers for these guys, just to double verify these get deleted
        typedef _smart_ptr<EntityInfoNode> EntityInfoNodePtr;

        // A small class to build a quick tree structure to represent the entity hierarchy
        class EntityInfoNode
            : public _reference_target_t
        {
        public:
            EntityInfoNode(const AZ::EntityId &entityId, EntityInfoNodePtr parentNode)
                : m_entityId(entityId), m_parent(parentNode), m_descendantCount(0) {}

            // The entity id of the ent on this node
            AZ::EntityId m_entityId;
            // The Node Ptr to the parent node ( where we can get their ID and such )
            EntityInfoNodePtr m_parent;
            // A Vector of our children via their nodes
            AZStd::vector<EntityInfoNodePtr> m_children;
            // A Cached count of this node's descendants, created at hierarchy creation time
            int m_descendantCount;
            // A Cached value of what is considered this entity's highest priority component to debug
            AZ::TypeId m_highestPriorityComponentDebug;
        };


        // A small struct to store common items used for an Entity Outliner Display Option
        struct EntOutlineDisplayOption
        {
            EntOutlineDisplayOption(bool enabled, const ImVec4 color)
                : m_enabled(enabled), m_color(color) {}
            bool m_enabled;
            ImVec4 m_color;
        };


        int m_totalEntitiesFound;
        EntityInfoNodePtr m_rootEntityInfo;
        AZStd::map<AZ::EntityId, EntityInfoNodePtr> m_entityIdToInfoNodePtrMap;
        // Delete the provided entity info struct pointer and any children recursively
        void DeleteEntityInfoAndDecendants(EntityInfoNodePtr entityInfo);
        EntityInfoNodePtr FindEntityInfoByEntityId(const AZ::EntityId &entityId, EntityInfoNodePtr searchNode);
        // Grab the root slice and re figure out the entity hierarchy
        void RefreshEntityHierarchy();
        int RefreshEntityHierarchy_FillCacheAndSort(EntityInfoNodePtr entityInfo);

        // Update Helper function to recursively draw and entity info and its children
        void ImGuiUpdate_RecursivelyDisplayEntityInfoAndDecendants(EntityInfoNodePtr node, bool justDrawChildren = false, bool drawInspectButton = true, bool drawTargetButton = true, bool drawDebugButton = true, bool sameLine = true, bool drawComponents = false);
        void ImGuiUpdate_RecursivelyDisplayEntityInfoAndDecendants_DrawDisplayOptions(EntityInfoNodePtr node, bool drawInspectButton, bool drawTargetButton, bool drawDebugButton, bool sameLine, bool drawComponents);

        bool ImGuiUpdate_DrawEntityView(const AZ::EntityId &ent);
        void ImGuiUpdate_DrawComponent(void *instance, const AZ::SerializeContext::ClassData *classData, const AZ::SerializeContext::ClassElement *classElement);

        bool ImGuiUpdate_DrawComponentView(const ImGui::ImGuiEntComponentId &entCom);       

        // Different options for the Outliner Display
        bool m_enabled;
        bool m_drawTargetViewButton;

        // Display options
        EntOutlineDisplayOption m_displayName;
        EntOutlineDisplayOption m_displayChildCount;
        EntOutlineDisplayOption m_displayDescentdantCount;
        EntOutlineDisplayOption m_displayParentInfo;
        EntOutlineDisplayOption m_displayLocalPos;
        EntOutlineDisplayOption m_displayLocalRotation;
        EntOutlineDisplayOption m_displayWorldPos;
        EntOutlineDisplayOption m_displayWorldRotation;

        // Quick enum for IMGui drop down menu 
        enum class HierarchyUpdateType : int
        {
            Constant = 0,
            UpdateTick,
        };
        HierarchyUpdateType m_hierarchyUpdateType;
        float m_hierarchyUpdateTickTimeCurrent;
        float m_hierarchyUpdateTickTimeTotal;

        // Sets of entity and component view windows
        AZStd::unordered_set<AZ::EntityId> m_entitiesToView;
        AZStd::unordered_set<ImGui::ImGuiEntComponentId> m_componentsToView;

        // data and interface for dealing with component debug windows and priorities
        typedef AZStd::pair<AZ::TypeId, int> ComponentPriority;
        AZStd::vector<ComponentPriority> m_componentDebugPriorities;
        
        bool ComponentHasDebug(const AZ::TypeId& comType);
        int GetComponentDebugPriority(const AZ::TypeId& comType);
        AZ::TypeId GetHighestPriorityDebugComponent();
    };
}

#endif // IMGUI_ENABLED
