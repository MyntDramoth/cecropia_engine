#pragma once
#include <queue>
#include <assert.h>
#include <array>
#include "ce_entity.h"


class Entity_Manager {
    public:
        Entity_Manager(){
            // Initialize the queue with all possible entity IDs
            for (entity entity = 0; entity < MAX_ENTITIES; ++entity)
            {
                available_entites.push(entity);
            }
            // Initialize the queue with all possible voxel IDs
            for (voxel voxel = 0; voxel < MAX_VOXELS; ++voxel)
            {
                available_voxels.push(voxel);
            }
        }

        entity create_entity()
        {
            assert(living_entity_count < MAX_ENTITIES && "Too many entities in existence.");

            // Take an ID from the front of the queue
            entity id = available_entites.front();
            available_entites.pop();
            ++living_entity_count;

            return id;
        }

        voxel create_voxel()
        {
            assert(living_voxel_count < MAX_VOXELS && "Too many voxels in existence.");

            // Take an ID from the front of the queue
            voxel id = available_voxels.front();
            available_voxels.pop();
            ++living_voxel_count;

            return id;
        }

        void destroy_entity(entity entity)
        {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Invalidate the destroyed entity's signature
            entity_signatures[entity].reset();

            // Put the destroyed ID at the back of the queue
            available_entites.push(entity);
            --living_entity_count;
        }

        void destroy_voxel(voxel voxel)
        {
            assert(voxel < MAX_VOXELS && "Voxel out of range.");

            // Invalidate the destroyed voxel's signature
            voxel_signatures[voxel].reset();

            // Put the destroyed ID at the back of the queue
            available_voxels.push(voxel);
            --living_voxel_count;
        }

        void set_signature(entity entity, signature signature)
        {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Put this entity's signature into the array
            entity_signatures[entity] = signature;
        }

        void set_vox_signature(voxel voxel, vox_signature vox_signature) {
            assert(voxel < MAX_VOXELS && "Voxel out of range.");

            // Put this voxel's signature into the array
            voxel_signatures[voxel] = vox_signature;
        }

        signature get_signature(entity entity) {
            assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Get this entity's signature from the array
            return entity_signatures[entity];
        }

        vox_signature get_vox_signature(voxel voxel) {
             assert(voxel < MAX_VOXELS && "Voxel out of range.");

            // Get this voxel's signature from the array
            return voxel_signatures[voxel];
        }


    private:
	// Queue of unused entity IDs
	std::queue<entity> available_entites{};
    std::queue<voxel> available_voxels{};

	// Array of signatures where the index corresponds to the entity ID
	std::array<signature, MAX_ENTITIES> entity_signatures{};
    std::array<vox_signature, MAX_VOXELS> voxel_signatures{};

	// Total living entities - used to keep limits on how many exist
	uint32_t living_entity_count{};
    uint32_t living_voxel_count{};
};