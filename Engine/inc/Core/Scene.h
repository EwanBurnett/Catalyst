#pragma once
#include <vector>

#include "../Entity/ECS.h"
#include "../Entity/Components.h"

namespace Engine {

    class Scene {

    public:
        struct Node
        {
        public:
            Node(Node* fromParent = nullptr);
            ~Node()
            {
                for (auto node : children)
                {
                    delete(node);
                }
            }
            Node* AddChild(Node* parent = nullptr)
            {
                if(parent == nullptr)
                {
                    parent = this;
                }
                Node* child = new Node(parent);
                children.push_back(child);
                return child;
            }

            void RemoveChild(Node* node)
            {
            }

            Entity entity;
            Node* parent;
            std::vector<Node*> children;
        };

        Node* Root()
        {
            return m_SceneRoot;
        }
        void Init()
        {
            m_SceneRoot = new Node();
        }
        void Shutdown()
        {
            delete m_SceneRoot;
        }

    private:
        Node* m_SceneRoot = nullptr;

    };

    inline Scene::Node::Node(Node* fromParent)
    {
        entity = ECS::Spawn();
        ECS::AddComponent<Transform>(entity);
        parent = fromParent;
    }
}

