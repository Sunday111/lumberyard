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

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>

namespace LmbrCentral
{
    /**
     * Base type to be used to do custom component debug drawing.
     */
    class EntityDebugDisplayComponent
        : public AZ::Component
        , private AZ::TransformNotificationBus::Handler
        , private AzFramework::EntityDebugDisplayEventBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(EntityDebugDisplayComponent, AZ::SystemAllocator, 0);
        AZ_RTTI(EntityDebugDisplayComponent, "{091EA609-13E9-4553-83BA-36878CBAB950}", AZ::Component);

        EntityDebugDisplayComponent() = default;

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        static void Reflect(AZ::ReflectContext* context);

    protected:
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("TransformService", 0x8ee22c50));
        }

        // AZ::TransformNotificationBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
        
        /**
         * Interface to draw using EntityDebugDisplayRequests API.
         */
        virtual void Draw(AzFramework::EntityDebugDisplayRequests* displayContext) = 0;

        const AZ::Transform& GetCurrentTransform() const { return m_currentEntityTransform; }
    
    private:
        AZ_DISABLE_COPY_MOVE(EntityDebugDisplayComponent)

        // AzFramework::EntityDebugDisplayEventBus
        void DisplayEntity(bool& handled) override;

        AZ::Transform m_currentEntityTransform; ///< Stores the transform of the entity.
    };
}