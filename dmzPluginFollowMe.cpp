#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include "dmzPluginFollowMe.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginFollowMe::PluginFollowMe (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _hil (0),
      _hilAttrHandle (0) {

   _init (local);
}


dmz::PluginFollowMe::~PluginFollowMe () {

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


// Object Observer Interface
void
dmz::PluginFollowMe::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _hilAttrHandle) {

      if (Value) {

         _hil = ObjectHandle;
         _log.warn << "Discovered Human in the Loop Handle: " << _hil << endl;
      }
      else if (ObjectHandle == _hil) {

         _hil = 0;
      }
   }
}


// PluginFollowMe Interface
void
dmz::PluginFollowMe::_init (Config &local) {

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);
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
