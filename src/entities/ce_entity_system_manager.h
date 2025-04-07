#pragma once

#include "ce_entity_system.h"
#include "ce_entity.h"
#include "../types.h"
#include "../rendering\my_engine_device.h"
#include <cassert>
#include <memory>
#include <unordered_map>

enum class sytem_type {
    Render,  // Example system types
    Physics,
    // Add more system types as needed
};

class System_Manager
{
    public:
	template<typename T>
	std::shared_ptr<T> register_system(My_Engine_Device& device)
	{
		u32 type_name = (u32)typeid(T).name();
		

		assert(systems.find(type_name) == systems.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		auto sys = std::make_shared<T>(device);
		
		//systems.insert({type_name, sys});
		systems.emplace( type_name, sys );
		
		return sys;
	}

	template<typename T>
	void set_entity_signature(signature signature)
	{
		u32 type_name = (u32)typeid(T).name();

		assert(systems.find(type_name) != systems.end() && "System used before registered.");

		// Set the signature for this system
		//signatures.insert({type_name, signature});
		signatures.emplace(type_name, signature);
	}

    template<typename T>
	void set_voxel_signature(vox_signature signature)
	{
		u32 type_name = (u32)typeid(T).name();

		assert(systems.find(type_name) != systems.end() && "System used before registered.");

		// Set the signature for this system
		//signatures.insert({type_name, signature});
		signatures.emplace(type_name, signature);
	}

    void entity_destroyed(entity entity)
	{
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : systems)
		{
			auto& system = pair.second;

			system->entities.erase(entity);
		}
	}

    void voxel_destroyed(voxel voxel)
	{
		// Erase a destroyed voxel from all system lists
		// voxels is a set so no check needed
		for (auto const& pair : systems)
		{
			auto& system = pair.second;

			system->voxels.erase(voxel);
		}
	}

    void entity_signature_changed(entity entity, signature entity_signature)
	{
		// Notify each system that an entity's signature changed
		for (auto const& pair : systems)
		{
			auto& type = pair.first;
			auto& system = pair.second;
			auto& system_signature = signatures[type];

			// Entity signature matches system signature - insert into set
			if ((entity_signature & system_signature) == system_signature)
			{
				system->entities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else
			{
				system->entities.erase(entity);
			}
		}
	}

    void voxel_signature_changed(voxel voxel, vox_signature voxel_signature)
	{
		// Notify each system that an entity's signature changed
		for (auto const& pair : systems)
		{
			auto& type = pair.first;
			auto& system = pair.second;
			auto& system_signature = vox_signatures[type];

			// voxel signature matches system signature - insert into set
			if ((voxel_signature & system_signature) == system_signature)
			{
				system->voxels.insert(voxel);
			}
			// voxel signature does not match system signature - erase from set
			else
			{
				system->voxels.erase(voxel);
			}
		}
	}

    private:
        // Map from system type string pointer to a signature
        std::unordered_map<u32, signature> signatures{};
        std::unordered_map<u32, vox_signature> vox_signatures{};

        // Map from system type string pointer to a system pointer
        std::unordered_map<u32, std::shared_ptr<System>> systems{}; 

};