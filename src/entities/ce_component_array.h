#pragma once
#include <queue>
#include <assert.h>
#include <array>
#include "ce_entity.h"
#include <unordered_map>

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
class I_Component_Array
{
	public:
		virtual ~I_Component_Array() = default;
		virtual void entity_destroyed(entity entity) = 0;
		virtual void voxel_destroyed(voxel voxel) = 0;
};

template<typename T>
class Component_Array : public I_Component_Array {
	public:
		void insert_entity_data(entity entity_in, T component)
		{
			assert(entity_to_index_map.find(entity_in) == entity_to_index_map.end() && "Component added to same entity more than once.");

			// Put new entry at end and update the maps
			size_t new_index = entity_size;
			entity_to_index_map[entity_in] = new_index;
			index_to_entity_map[new_index] = entity_in;
			entity_component_array[new_index] = component;
			++entity_size;
		}

		void insert_voxel_data(voxel vox, T component)
		{
			assert(entity_to_index_map.find(vox) == entity_to_index_map.end() && "Component added to same entity more than once.");

			// Put new entry at end and update the maps
			size_t new_index = voxel_size;
			voxel_to_index_map[vox] = new_index;
			index_to_voxel_map[new_index] = vox;
			vox_component_array[new_index] = component;
			++voxel_size;
		}

		void remove_entity_data(entity entity_in)
		{
			assert(entity_to_index_map.find(entity) != entity_to_index_map.end() && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			size_t index_of_removed_entity = entity_to_index_map[entity_in];
			size_t index_of_last_element = entity_size - 1;
			entity_component_array[index_of_removed_entity] = entity_component_array[index_of_last_element];

			// Update map to point to moved spot
			entity entity_of_last_element = index_to_entity_map[index_of_last_element];
			entity_to_index_map[entity_of_last_element] = index_of_removed_entity;
			index_to_entity_map[index_of_removed_entity] = entity_of_last_element;

			entity_to_index_map.erase(entity_in);
			index_to_entity_map.erase(index_of_last_element);

			--entity_size;
		}

		void remove_voxel_data(voxel vox)
		{
			
			assert(voxel_to_index_map.find(voxel) != voxel_to_index_map.end() && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			size_t index_of_removed_voxel = voxel_to_index_map[vox];
			size_t index_of_last_element = voxel_size - 1;
			vox_component_array[index_of_removed_voxel] = vox_component_array[index_of_last_element];

			// Update map to point to moved spot
			voxel voxel_of_last_element = index_to_voxel_map[index_of_last_element];
			voxel_to_index_map[voxel_of_last_element] = index_of_removed_voxel;
			index_to_voxel_map[index_of_removed_voxel] = voxel_of_last_element;

			voxel_to_index_map.erase(vox);
			index_to_voxel_map.erase(index_of_last_element);

			--voxel_size;
		}

		T& get_entity_data(entity entity)
		{
			assert(entity_to_index_map.find(entity) != entity_to_index_map.end() && "Retrieving non-existent component.");

			// Return a reference to the entity's component
			return entity_component_array[entity_to_index_map[entity]];
		}

		T& get_voxel_data(voxel voxel)
		{
			assert(voxel_to_index_map.find(voxel) != voxel_to_index_map.end() && "Retrieving non-existent component.");

			// Return a reference to the voxel's component
			return vox_component_array[voxel_to_index_map[voxel]];
		}

		void entity_destroyed(entity entity) override
		{
			if (entity_to_index_map.find(entity) != entity_to_index_map.end())
			{
				// Remove the entity's component if it existed
				remove_entity_data(entity);
			}
		}

		void voxel_destroyed(voxel voxel) override
		{
			if (voxel_to_index_map.find(voxel) != voxel_to_index_map.end())
			{
				// Remove the voxel's component if it existed
				remove_voxel_data(voxel);
			}
		}
	private:
		// The packed array of components (of generic type T),
		// set to a specified maximum amount, matching the maximum number
		// of entities allowed to exist simultaneously, so that each entity
		// has a unique spot.
		std::array<T, MAX_ENTITIES> entity_component_array;
		std::array<T, MAX_VOXELS> vox_component_array;
		// Map from an entity ID to an array index.
		std::unordered_map<entity, size_t> entity_to_index_map;
		std::unordered_map<voxel, size_t> voxel_to_index_map;

		// Map from an array index to an entity ID.
		std::unordered_map<size_t, entity> index_to_entity_map;
		std::unordered_map<size_t, voxel> index_to_voxel_map;

		// Total size of valid entries in the array.
		size_t entity_size;
		size_t voxel_size;
};