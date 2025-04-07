#pragma once
#include <vector>
#include "physics/physics.h"
#include "physics/raycast.h"
#include "entities/ce_coordinator.h"
#include <iostream>



struct SVO_Node {
    AABB aabb;
    int depth {0};
    SVO_Node* parent;
    SVO_Node* children[8];
    std::vector<AABB> bounds;
    std::vector<voxel> voxel_components;
    bool is_empty = false;
    SVO_Node(const AABB aabb) : aabb(aabb) {
        for (int i = 0; i < 8; ++i) {
            children[i] = nullptr;
        }
    }
};


class SVO {
    public:
        SVO(const AABB aabb, int max_depth = 32);
        


        void insert(const AABB aabb, voxel voxel_id);
        void query_range(const AABB aabb,std::vector<AABB> &result);
        void query_full_nodes(const AABB aabb, std::vector<AABB>& result);
    private:
        SVO_Node* root_node;
        int max_depth;
        
        void insert_recursive(SVO_Node* node,const AABB aabb, voxel voxel_id, int depth);
        void queary_range_recursive(SVO_Node* node, const AABB aabb, std::vector<AABB> &result);
        void query_full_nodes_recursive(SVO_Node* node, const AABB aabb, std::vector<AABB>& result);
        bool ray_cast_recursive(SVO_Node* node, const Ray& ray, AABB& resultBounds, std::vector<voxel>& resultVoxels);
};