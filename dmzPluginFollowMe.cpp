#include "dmzPluginFollowMe.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginFollowMe::PluginFollowMe (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      _log (Info) {

   _log.warn << "Creating Plugin: " << Info.get_name () << endl;
   _init (local);
}


dmz::PluginFollowMe::~PluginFollowMe () {

   _log.warn << "Deleting Plugin: " << get_plugin_name () << endl;
}


// Plugin Interface
void
dmz::PluginFollowMe::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::PluginFollowMe::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// PluginFollowMe Interface
void
dmz::PluginFollowMe::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginFollowMe (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginFollowMe (Info, local);
}

};
