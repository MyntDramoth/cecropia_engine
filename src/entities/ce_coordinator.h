#pragma once
#include "ce_component_manager.h"
#include "ce_entity_manager.h"
#include "ce_entity_system_manager.h"

class Coordinator
{
    public:
	void Init() {
		// Create pointers to each manager
		component_manager = std::make_unique<Component_Manager>();
		entity_manager = std::make_unique<Entity_Manager>();
		system_manager = std::make_unique<System_Manager>();
	}

    // Entity methods
	entity create_entity() {
		return entity_manager->create_entity();
	}

    voxel create_voxel() {
		return entity_manager->create_voxel();
	}

	void destroy_entity(entity entity) {
		entity_manager->destroy_entity(entity);

		component_manager->entity_destroyed(entity);

		system_manager->entity_destroyed(entity);
	}

    void destroy_voxel(voxel voxel) {
		entity_manager->destroy_voxel(voxel);

		component_manager->voxel_destroyed(voxel);

		system_manager->voxel_destroyed(voxel);
	}

    // Component methods
	template<typename T>
	void register_entity_component() {
		component_manager->register_component<T>();
	}

    template<typename T>
	void register_voxel_component() {
		component_manager->register_vox_component<T>();
	}

    template<typename T>
	void entity_add_component(entity entity, T component) {
        component_manager->add_entity_component(entity, component);
		auto signature = entity_manager->get_signature(entity);
		signature.set(component_manager->get_component_type<T>(), true);
		entity_manager->set_signature(entity, signature);

		system_manager->entity_signature_changed(entity, signature);
	}

    template<typename T>
	void voxel_add_component(voxel voxel, T component) {
        component_manager->add_voxel_component(voxel, component);
		auto signature = entity_manager->get_vox_signature(voxel);
		signature.set(component_manager->get_vox_component_type<T>(), true);
		entity_manager->set_vox_signature(voxel, signature);

		system_manager->voxel_signature_changed(voxel, signature);
	}

    template<typename T>
	void entity_remove_component(entity entity) {
		component_manager->remove_entity_component<T>(entity);

		auto signature = entity_manager->get_signature(entity);
		signature.set(component_manager->get_component_type<T>(), false);
		entity_manager->set_signature(entity, signature);

		system_manager->entity_signature_changed(entity, signature);
	}

    template<typename T>
	void voxel_remove_component(voxel voxel) {
		component_manager->remove_voxel_component<T>(voxel);

		auto signature = entity_manager->get_vox_signature(voxel);
		signature.set(component_manager->get_vox_component_type<T>(), false);
		entity_manager->set_vox_signature(voxel, signature);

		system_manager->voxel_signature_changed(voxel, signature);
	}

    template<typename T>
	T& entity_get_component(entity entity) {
		return component_manager->get_entity_component<T>(entity);
	}

    template<typename T>
	T& voxel_get_component(voxel voxel) {
		return component_manager->get_voxel_component<T>(voxel);
	}

    template<typename T>
	component_type entity_get_component_type() {
		return component_manager->get_component_type<T>();
	}

     template<typename T>
	vox_component_type voxel_get_component_type() {
		return component_manager->get_vox_component_type<T>();
	}

    // System methods
	template<typename T>
	std::shared_ptr<T> register_system(My_Engine_Device& device)
	{
		return system_manager->register_system<T>(device);
	}

	template<typename T>
	void entity_set_system_signature(signature signature)
	{
		system_manager->set_entity_signature<T>(signature);
	}

    template<typename T>
	void voxel_set_system_signature(signature signature)
	{
		system_manager->set_voxel_signature<T>(signature);
	}

    private:
        std::unique_ptr<Component_Manager> component_manager;
        std::unique_ptr<Entity_Manager> entity_manager;
        std::unique_ptr<System_Manager> system_manager;
};