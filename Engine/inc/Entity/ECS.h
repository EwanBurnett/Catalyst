#pragma once

#include <entt/entt.hpp>    //TODO: Remove entt dependency

namespace Engine
{
    struct Entity
    {
        entt::entity _entity;
    };

    namespace ECS {
        entt::registry m_Registry;

        /**
         * \brief Spawns an entity
         * \return The spawned entity
         */
        static Entity Spawn()
        {
            return { m_Registry.create() };
        }

        /**
         * \brief Despawns an entity.
         * \param entity The entity to despawn
         */
        static void Despawn(Entity& entity)
        {
            m_Registry.destroy(entity._entity);
        }

        /**
         * \brief Adds a component to an entity
         * \tparam The type of component to add
         * \param entity The entity to add the component to
         * \param data (Optional) data to initialize the component with
         */
        template<typename T>
        static void AddComponent(Entity& entity, T data = {})
        {
            //if (m_Registry.valid(entity._entity)) {
                m_Registry.emplace<T>(entity._entity, data);
            //}
        }

        /**
         * \brief Retrieve a specific component from an entity.
         * \tparam T Component type to retrieve
         * \param e The Entity to query
         * \return a Reference to the component data.
         */
        template<typename T>
        static T* GetComponent(Entity& e)
        {
            auto view = m_Registry.view<T>();
            return &view.get<T>(e._entity);

        }


        /**
         * \brief Queries the ECS for entities with specific components.
         * \tparam T Component Types to retrieve
         * \return A std::vector of entities with the specified components
         */
        template<typename ... T>
        static std::vector<Entity> GetEntitiesWithComponents()
        {
            std::vector<Entity> vec;

            auto view = m_Registry.view<T ...>();

            view.each([&](const auto entity, const auto& ...)
                {
                    vec.emplace_back(entity);
                });
            return vec;

        }



    }
}

