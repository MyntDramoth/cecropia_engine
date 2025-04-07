#pragma once
#include "rendering\crec_window.h"
#include "rendering\my_engine_device.h"
#include "ce_game_object.h"
#include "rendering\ce_renderer.h"
#include "rendering\ce_descriptor_pool.h"
#include "entities\ce_coordinator.h"
#include "rendering\ce_texture.h"

#include <memory>
#include <vector>

Coordinator global_coordinator;

class Program {
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 800;

	Program();
	~Program();

	Program(const Program&) = delete;
	Program &operator=(const Program&) = delete;



	void run();
private:
	void load_game_objects();	
	void load_entites();
	CE_Window ce_window{ WIDTH, HEIGHT, "Crecopia Engine" };
	My_Engine_Device ce_device{ ce_window };
	CE_Renderer ce_renderer{ ce_window,ce_device };
	//CE_Frame_Info info;

	//std::unordered_map<std::shared_ptr<CE_Model>, CE_Image::Texture*> materials;
	//std::unique_ptr<CE_Descriptor_Set_Layout> layout;
	// note: order of declaration matters
	std::unique_ptr<CE_Descriptor_Pool> global_pool{};
	CE_Game_Object::map game_objects;
	Vox::voxels used_voxels;
};