
#include "lv2.h"
#include "engine.h"

#include <stdexcept>

namespace jiss {

void check_zero(const void *p) {
	if (0 == p) throw std::runtime_error("something something, rabble rabble");
}

lv2::lv2(const std::string &uri, engine &e) {
		jdbg("lv2(" << uri << ")")
		const LilvPlugins *plugins = lilv_world_get_all_plugins(e.lilv_world);
		check_zero(plugins);

		LilvNode *uri_node = lilv_new_uri(e.lilv_world, uri.c_str());
		check_zero(uri_node);
		
		const LilvPlugin *plugin = lilv_plugins_get_by_uri(plugins, uri_node);
		check_zero(plugin);
	
		instance = lilv_plugin_instantiate(plugin, e.get_samplerate(), 0);
		check_zero(instance);
	
		jdbg(instance)
		lilv_instance_activate(instance);
//		plugin = lilv_world_get_plugin(
}

} // namespace
