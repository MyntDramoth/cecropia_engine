#include "svo.h"

SVO::SVO(const AABB aabb, int max_depth) {
    root_node = new SVO_Node(aabb);
    this->max_depth = max_depth;
    root_node->depth = 0;  
}



//this determines where an item should go in the octree when inserted
void SVO::insert(const AABB aabb, voxel voxel_id) {
    insert_recursive(root_node,aabb,voxel_id,0);
}

void SVO::insert_recursive(SVO_Node* node,const AABB aabb, voxel voxel_id, int depth) {
    if(depth == max_depth || node == nullptr || node == root_node) {
        node->bounds.push_back(aabb);
        node->voxel_components.push_back(voxel_id);
        return;
    }

    int idx = 0;
    //gets which part of the octant to insert a node into checks if the point is above or below the center
    
    if(aabb.position.x >= node->aabb.position.x) idx |= 1;
    if(aabb.position.y >= node->aabb.position.y) idx |= 2;
    if(aabb.position.z >= node->aabb.position.z) idx |= 4;

    //std::cout<<"idx: "<<idx<<std::endl;

    
    if(node->children[idx] == nullptr) {
        AABB child_bounds;
        glm::vec3 child_half_size {node->aabb.half_size.x * 0.5f,node->aabb.half_size.y * 0.5f,node->aabb.half_size.z * 0.5f};

        glm::vec3 child_position(
            node->aabb.position.x + (aabb.position.x >= node->aabb.position.x ? child_half_size.x : -child_half_size.x),
            node->aabb.position.y + (aabb.position.y >= node->aabb.position.y ? child_half_size.y : -child_half_size.y),
            node->aabb.position.z + (aabb.position.z >= node->aabb.position.z ? child_half_size.z : -child_half_size.z)
        );
        child_bounds.position = child_position;
        child_bounds.half_size = child_half_size;
        node->children[idx] = new SVO_Node(child_bounds);
    }
    insert_recursive(node->children[idx],aabb, voxel_id, depth+1);
}

void SVO::query_range(const AABB aabb,std::vector<AABB> &result) {
    queary_range_recursive(root_node,aabb,result);
}

void SVO::queary_range_recursive(SVO_Node* node, const AABB aabb, std::vector<AABB> &result) {
    if(node == nullptr) {return;}
    // Check if the node's bounding box intersects with the query range
    if(node->aabb.half_size.x < aabb.half_size.x || node->aabb.half_size.y < aabb.half_size.y || node->aabb.half_size.z < aabb.half_size.z)
    {return;}

    
    // If the node's bounding box is completely contained within the query range, add all points in this node to the result
    if(node->aabb.half_size.x >= aabb.half_size.x && node->aabb.half_size.y >= aabb.half_size.y && node->aabb.half_size.z >= aabb.half_size.z) {
        result.insert(result.end(),node->bounds.begin(),node->bounds.end());
        return;
    }

    // Recursively check child nodes
    for (int i = 0; i < 8; ++i) {
        queary_range_recursive(node->children[i], aabb, result);
    }
}

void SVO::query_full_nodes(const AABB aabb, std::vector<AABB>& result) {
    query_full_nodes_recursive(root_node,aabb,result);
}

void SVO::query_full_nodes_recursive(SVO_Node* node, const AABB aabb, std::vector<AABB>& result) {
    if (node == nullptr) {
        return;  // Stop if the node is null.
    }

    // Check if the node is a leaf node and it's full (contains data).
    if (node->depth == max_depth && !node->bounds.empty()) {
        result.insert(result.end(), node->bounds.begin(), node->bounds.end());
        return;  // Stop further traversal.
    }

    // Check if all child nodes are full.
    bool allChildrenFull = true;
    for (int i = 0; i < 8; ++i) {
        if (!node->children[i] || (!node->children[i]->depth == max_depth && node->children[i]->bounds.empty())) {
            allChildrenFull = false;
            break;
        }
    }

    if (allChildrenFull) {
        // If all child nodes are full, add this node's bounds to the result.
        result.insert(result.end(), node->bounds.begin(), node->bounds.end());
        return;  // Stop further traversal.
    }

    // Continue recursively checking child nodes.
    for (int i = 0; i < 8; ++i) {
        query_full_nodes_recursive(node->children[i], aabb ,result);
    }
}

bool SVO::ray_cast_recursive(SVO_Node* node, const Ray& ray, AABB& resultBounds, std::vector<voxel>& resultVoxels) {
    if (node == nullptr) {
        return false;  // Stop if the node is null.
    }


    if(node->aabb.half_size.x < ray.ray_bounds.half_size.x || node->aabb.half_size.y < ray.ray_bounds.half_size.y || node->aabb.half_size.z < ray.ray_bounds.half_size.z) {

    }
    /*
    // Perform ray intersection test with the node's bounding box.
    if (!ray.intersectsAABB(node->aabb)) {
        return false;  // Ray doesn't intersect this node, stop traversal.
    }

    // Check if the node is a leaf node and it's full (contains data).
    if (node->isLeaf() && !node->bounds.empty()) {
        resultBounds = node->aabb;
        resultVoxels = node->voxel_components;
        return true;  // Ray intersects a full leaf node, stop traversal.
    }
    */
    // Check if all child nodes are full.
    bool allChildrenFull = true;
    for (int i = 0; i < 8; ++i) {
        if (!ray_cast_recursive(node->children[i], ray, resultBounds, resultVoxels)) {
            allChildrenFull = false;
        }
    }

    if (allChildrenFull) {
        resultBounds = node->aabb;
        resultVoxels = node->voxel_components;
        return true;  // Ray intersects a node with all full child nodes, stop traversal.
    }

    return false;  // Ray intersects this node but none of the above cases, continue traversal.
}