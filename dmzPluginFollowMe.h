#ifndef DMZ_PLUGIN_FOLLOW_ME_DOT_H
#define DMZ_PLUGIN_FOLLOW_ME_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesMask.h>
#include <dmzTypesVector.h>

namespace dmz {

   class PluginFollowMe :
         public Plugin {

      public:
         PluginFollowMe (const PluginInfo &Info, Config &local);
         ~PluginFollowMe ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         PluginFollowMe ();
         PluginFollowMe (const PluginFollowMe &);
         PluginFollowMe &operator= (const PluginFollowMe &);
   };
};

#endif // DMZ_PLUGIN_FOLLOW_ME_DOT_H
