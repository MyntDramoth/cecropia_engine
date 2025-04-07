#pragma once
#include "ce_entity.h"
#include "ce_component_array.h"
#include <memory>

class Component_Manager {
    public:
        template<typename T>
        void register_component()
        {
            const char* type_name = typeid(T).name();

            assert(component_types.find(type_name) == component_types.end() && "Registering component type more than once.");

            // Add this component type to the component type map
            component_types.insert({type_name, next_component_type});

            // Create a ComponentArray pointer and add it to the component arrays map
            component_arrays.insert({type_name, std::make_shared<Component_Array<T>>()});

            // Increment the value so that the next component registered will be different
            ++next_component_type;
        }

        template<typename T>
        void register_vox_component()
        {
            const char* type_name = typeid(T).name();

            assert(vox_component_types.find(type_name) == vox_component_types.end() && "Registering component type more than once.");

            // Add this component type to the component type map
            vox_component_types.insert({type_name, next_vox_component_type});

            // Create a ComponentArray pointer and add it to the component arrays map
            vox_component_arrays.insert({type_name, std::make_shared<Component_Array<T>>()});

            // Increment the value so that the next component registered will be different
            ++next_vox_component_type;
        }

        template<typename T>
        component_type get_component_type()
        {
            const char* type_name = typeid(T).name();

            assert(component_types.find(type_name) != component_types.end() && "Component not registered before use.");

            // Return this component's type - used for creating signatures
            return component_types[type_name];
        }

        template<typename T>
        component_type get_vox_component_type()
        {
            const char* type_name = typeid(T).name();

            assert(vox_component_types.find(type_name) != vox_component_types.end() && "Component not registered before use.");

            // Return this component's type - used for creating signatures
            return vox_component_types[type_name];
        }

        template<typename T>
        void add_entity_component(entity entity, T component)
        {
            // Add a component to the array for an entity
            get_component_array<T>()->insert_entity_data(entity, component);
        }

        template<typename T>
        void add_voxel_component(voxel voxel, T component)
        {
            // Add a component to the array for an entity
            get_vox_component_array<T>()->insert_voxel_data(voxel, component);
        }

        template<typename T>
        void remove_entity_component(entity entity)
        {
            // Remove a component from the array for an entity
            get_component_array<T>()->remove_entity_data(entity);
        }

        template<typename T>
        void remove_voxel_component(voxel voxel)
        {
            // Remove a component from the array for an entity
            get_vox_component_array<T>()->remove_voxel_data(voxel);
        }

        template<typename T>
        T& get_entity_component(entity entity)
        {
            // Get a reference to a component from the array for an entity
            return get_component_array<T>()->get_entity_data(entity);
        }

        template<typename T>
        T& get_voxel_component(voxel voxel)
        {
            // Get a reference to a component from the array for an entity
            return get_vox_component_array<T>()->get_voxel_data(voxel);
        }

        void entity_destroyed(entity entity)
        {
            // Notify each component array that an entity has been destroyed
            // If it has a component for that entity, it will remove it
            for (auto const& pair : component_arrays)
            {
                auto const& component = pair.second;

                component->entity_destroyed(entity);
            }
        }

        void voxel_destroyed(voxel voxel)
        {
            // Notify each voxel array that a voxel has been destroyed
            // If it has a component for that voxel, it will remove it
            for (auto const& pair : component_arrays)
            {
                auto const& component = pair.second;

                component->voxel_destroyed(voxel);
            }
        }

    private:
	// Map from type string pointer to a component type
	std::unordered_map<const char*, component_type> component_types{};
    std::unordered_map<const char*, vox_component_type> vox_component_types{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<I_Component_Array>> component_arrays{};
    std::unordered_map<const char*, std::shared_ptr<I_Component_Array>> vox_component_arrays{};
	// The component type to be assigned to the next registered component - starting at 0
	component_type next_component_type{};
    vox_component_type next_vox_component_type{};
	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
	template<typename T>
	std::shared_ptr<Component_Array<T>> get_component_array()
	{
		const char* type_name = typeid(T).name();

		assert(component_types.find(type_name) != component_types.end() && "Component not registered before use.");

		return std::static_pointer_cast<Component_Array<T>>(component_arrays[type_name]);
	}

    template<typename T>
	std::shared_ptr<Component_Array<T>> get_vox_component_array()
	{
		const char* type_name = typeid(T).name();

		assert(vox_component_types.find(type_name) != vox_component_types.end() && "Component not registered before use.");

		return std::static_pointer_cast<Component_Array<T>>(vox_component_arrays[type_name]);
	}
};