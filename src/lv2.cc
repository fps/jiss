
#include "lv2.h"
#include "engine.h"

namespace jiss {

lv2::lv2(const std::string &uri, engine &e) {
		jdbg("lv2(" << uri << ")")
		const LilvPlugins *plugins = lilv_world_get_all_plugins(e.lilv_world);
		LilvNode *uri_node = lilv_new_uri(e.lilv_world, uri.c_str());
		const LilvPlugin *plugin = lilv_plugins_get_by_uri(plugins, uri_node);
		instance = lilv_plugin_instantiate(plugin, e.get_samplerate(), 0);
		jdbg(instance)
		lilv_instance_activate(instance);
//		plugin = lilv_world_get_plugin(
}

} // namespace
